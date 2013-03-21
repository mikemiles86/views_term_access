<?php
/**
 * @file
 * Definition of views_plugin_term_access class.
 */
 
/**
 * Access plugin that provides term-based access control.
 *
 * @ingroup views_access_plugins
 */
class views_plugin_access_term extends views_plugin_access {
  /**
   * Determine if the current user has access or not.
   */
  function access($account) {
    return views_term_access_check_terms($this->options['tids'], $this->options['term_matching']);
  }

  /**
   * Determine the access callback and arguments.
   *
   * This information will be embedded in the menu in order to reduce
   * performance hits during menu item access testing, which happens
   * a lot.
   *
   * @return an array; the first item should be the function to call,
   *   and the second item should be an array of arguments. The first
   *   item may also be TRUE (bool only) which will indicate no
   *   access control.)
   */
  function get_access_callback() {
    return array('views_term_access_check_terms', array($this->options['tids'], $this->options['term_matching']));
  }

  /**
   * Return a string to display as the clickable title for the
   * access control.
   */
  function summary_title() {
    $tids = $this->options['tids'];
    //if multiple terms were selected display 'Many Terms'
    if (sizeof($tids)>1) {
      return t('Many Terms') . ',' . $this->options['term_matching'];
    }
    //if single term selected, load term name
    elseif (sizeof($tids) == 1) {
      $term = taxonomy_term_load($tids[0]);
      return $term->name;
    }
    //default return 'none'
    return t('None');
  }

  /**
   * Retrieve the options when this is a new access
   * control plugin
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['tids'] = array();
    $options['term_matching'] = 'any';
    return $options;
  }

   /**
   * Provide the default form for setting options.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    //get all user accosicated vocabularies
    $vocabularies = views_term_access_get_user_vocabularies();
  
    $form['vocabularies'] = array(
      '#type' => 'fieldset',
      '#title' => t('Vocabularies'),
      '#description' => t('Select the terms that need to be matched to allow access.')
    );
    
    //foreach vocabulary create a multi-select field
    foreach ($vocabularies as $vid => $vocabulary) {
      $form['vocabularies'][$vid] = array(
        '#type' => 'select',
        '#title' => check_plain($vocabulary['name']),
        '#options' => array('' => '') + $vocabulary['terms'],
        '#default_value' => $this->options['tids'],
        '#description' => t('Used by fields') . ':' . implode(',', $vocabulary['fields']),
        '#multiple' => TRUE,
      );
    }
    
    $form['term_matching'] = array(
      '#type'  => 'select',
      '#title'  => t('User must be associated to'),
      '#options'  => array('all' => 'All Selected Terms', 'any' => 'Any Selected Terms'),
      '#default_value'  => $this->options['term_matching'],
    );
    
    $form['tids'] = array(
      '#type'  => 'value',
      '#value'  => $this->options['tids'],
    );
  }
	
  /**
   * Provide the default form form for submitting options
   */ 
  function options_submit(&$form, &$form_state) {
    //get all user associated taxonomy vocabularies
    $vocabularies = views_term_access_get_user_vocabularies();
    $tids = array();
    //loop through all of the vocabularies
    foreach ($vocabularies as $vid => $vocabulary) {
      //get selected terms
      if (array_filter($form_state['values']['access_options']['vocabularies'][$vid])) {
        $tids = array_merge($tids, array_keys($form_state['values']['access_options']['vocabularies'][$vid]));
      }
    }
    //set selected tids array to be saved.
    $form_state['values']['access_options']['tids'] = $tids;
  }
  
}