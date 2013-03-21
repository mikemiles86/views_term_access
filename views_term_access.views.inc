<?php
/**
 * @file
 * Contains implementations of views hooks
 */

/**
 * Implements hook_views_plugins().
 */
function views_term_access_views_plugins() {
  $plugins = array(
    'access' => array(
      'term_access' => array(
        'title'  => 'Term Access',
        'handler'  => 'views_plugin_access_term',
        'uses options'  => TRUE,
        'help'    => 'Access will be granted by user association to taxonomy terms',
      ),
    ),
  );
  return $plugins;
}