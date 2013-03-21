<?php
/**
 * @file
 * Contains none-hook functions used by the views_term_access module
 */

/**
 * performs check of user to see if they are associated to required terms.
 *
 * @param array $tids
 *	an array of taxonomy term ids
 * @param string $matching
 *	the method for how to match the terms to the user
 *
 * @return boolean
 *	true/false is user matches the terms correctly.
 */
function views_term_access_check_terms($tids, $matching='any') {
  global $user;
  $matched = 0;
  // make sure that terms have been selected
  if (sizeof($tids)>0) {
    // fetch the list of term reference fields associated to user entity
    $reference_fields = views_term_access_get_reference_fields();
    // load the full user object of the current logged in user
    $account = user_load($user->uid);
    // for each term reference field
    foreach ($reference_fields as $machine_name => $vids) {
      //get field value for this user
      if ($values = field_get_items('user', $account, $machine_name)) {
        // for each delta (since can be multiple terms
        foreach ($values as $delta => $value) {
          // if delta has a 'tid' key and matches one of the selected tids
          if (isset($value['tid']) && in_array($value['tid'], $tids)) {
            //increase matched count
            $matched++;
          }
        }
      }
    }
  }
  // only return true if either
  // 1. no tids have been selected [or]
  // 2. the user matches selected tids and either [and]
  //    a. matching method is 'any' [or]
  //    b. all of the terms have been matched
  if ( (sizeof($tids)==0) ||(($matched > 0) && (($matching == 'any') || (sizeof($tids)==$matched)))) {
    return TRUE;
  }
  // default, return false
  return FALSE;
}

/**
 * get list of taxonomy vocabularies that are tied to user  taxonomy term reference fields.
 *
 * @return array
 *	array of taxonomy vocabularies, their terms and the user taxonomy term reference fields
 *  to which they are associated.
 */
function views_term_access_get_user_vocabularies() {
  // retrieve a list of taxonomy term reference fields associated to user entity
  $reference_fields = views_term_access_get_reference_fields();

  //get all vocabulary vids
  $vids = array();
  foreach ($reference_fields as $ref_vids) {
    $vids=array_merge($vids, array_values($ref_vids));
  }
  // load all of the vocabularies needed
  $vocabularies = taxonomy_vocabulary_load_multiple($vids);
  
  $user_vocabularies = array();
  // loop through all of the retrieved vocabularies
  foreach ($vocabularies as $vid => $vocabulary) {
    // load vocabulary terms (if there are any)
    if ($tree = taxonomy_get_tree($vid)) {
      //get terms, only want the tid and name
      $terms = array();
      foreach ($tree as $term) {
        $terms[$term->tid] = $term->name;
      }
      
      $fields = array();
      // loop through the taxonomy reference fields to 
      // see which ones this vocabulary is associated to
      foreach ($reference_fields as $field_name => $vids) {
        if (in_array($vid, $vids)) {
          $fields[] = $field_name;
        }
      }
      // if vocabulary has terms
      if (sizeof($terms)>0) {
        // add it to the user terms as one that can be selected.
        $user_vocabularies[$vid] = array(
          'name' => $vocabulary->name,
          'terms' => $terms,
          'fields' => $fields,
        );
      }
    }
  }
  // return array of user vocabularies
  return $user_vocabularies;
}

/**
 * retrieves list of user taxonomy term reference fields
 *
 * @return array
 *	An array of taxonomy term reference fields associated to the user entity.
 *  key being the field_name and value an array of associated taxonomy
 *  vocabularies.
 */
function views_term_access_get_reference_fields() {
  //get all user term reference fields
  $reference_fields = array();
  // get info on all user entity fields
  $fields_info = field_info_instances('user', 'user');
  //loop through each field
  foreach ($fields_info as $field_name => $value) {
    //get additional info on field
    $field_info = field_info_field($field_name);
    //get field type
    $type = $field_info['type'];
    //check to see if the field type is a term reference field
    if ($type == 'taxonomy_term_reference') {
      //loop through the allowed values for field to retrieve vocabularies (could be multiple)
      foreach ($field_info['settings']['allowed_values'] as $vocab) {
        //if vocabulary is present, attempt to load based on machine name
        if (isset($vocab['vocabulary']) && ($vocab = taxonomy_vocabulary_machine_name_load($vocab['vocabulary']))) {
          //vocabulary loaded, add vid to reference fields array.
          $reference_fields[$field_name][] = $vocab->vid;
        }
      }
    }
  }
  //return array of user taxonomy reference fields
  return $reference_fields;
}