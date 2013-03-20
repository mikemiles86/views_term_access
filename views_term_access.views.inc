<?php
/**
 * @file
 * The views hook file
 *
 * Contains implementations of drupal views hooks
 */

/**
 * Instance of hook_views_plugins
 */
function views_term_access_views_plugins() {
  $plugins = array(
    'access' => array(
      'time_access' => array(
        'title'  => 'Term Access',
        'handler'  => 'views_plugin_term_access',
        'uses options'  => TRUE,
        'help'    => t('Access will be granted by association to specific terms'),
      ),
    ),
  );
  return $plugins;
}
  