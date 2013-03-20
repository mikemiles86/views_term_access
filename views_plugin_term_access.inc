<?php

class views_plugin_term_access extends views_plugin_access {
	function access($account){
		return views_term_access_check_terms($this->options['tids'],$this->options['term_matching']);
	}
	
	function get_access_callback(){
		return array('views_term_access_check_terms',array($this->options['tids'],$this->options['term_matching']));
	}

	function summary_title(){
		$tids = $this->options['tids'];
		
		if (sizeof($tids)>1){
			return t('Many Terms').','.$this->options['term_matching'];
		}else if (sizeof($tids) == 1){
			$term = taxonomy_term_load($tids[0]);
			return $term->name;
		}

		return t('None');
	}
	
	function option_definition() {
    $options = parent::option_definition();
    $options['tids'] = array();
		$options['term_matching'] = 'any';
    return $options;
  }
	
	function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
		
		//get all user accosicated vocabularies
		$vocabularies = views_term_access_get_user_vocabularies();
	
		$form['vocabularies'] = array(
			'#type'					=> 'fieldset',
			'#title'				=> t('Vocabularies'),
			'#description'	=> t('Select the terms that need to be matched to allow access.')
		);
		
		foreach ($vocabularies as $vid=>$vocabulary){
			$form['vocabularies']['v_'.$vid] = array(
				'#type'					=> 'select',
				'#title'				=> $vocabulary['name'],
				'#options'			=> array(''=>'')+$vocabulary['terms'],
				'#default_value'=> $this->options['tids'],
				'#description'	=> t('Used by fields').':'.implode(',',$vocabulary['fields']),
				'#multiple'			=> true,
			);
		}
		
		$form['term_matching'] = array(
			'#type'	=> 'select',
			'#title'	=> t('User must be associated to'),
			'#options'	=> array('all'=>'All Selected Terms','any'=>'Any Selected Terms'),
			'#default_value'	=> $this->options['term_matching'],
		);
		
		$form['tids'] = array(
			'#type'	=> 'value',
			'#value'	=> $this->options['tids'],
		);
  }
	
	function options_submit(&$form, &$form_state) {
	
		$vocabularies = views_term_access_get_user_vocabularies();
		$tids = array();
		foreach ($vocabularies as $vid => $vocabulary){
			//get selected terms
			if (array_filter($form_state['values']['access_options']['vocabularies']['v_'.$vid])){
				$tids = array_merge($tids, array_keys($form_state['values']['access_options']['vocabularies']['v_'.$vid]));
			}
		}
		
		$form_state['values']['access_options']['tids'] = $tids;
  }
	
}