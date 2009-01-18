<?php
// $Id$

/**
 * @file
 *
 * Provides views intergration for the me module.
 */

/**
 * Implementation of hook_views_data_alter().
 */
function me_views_data_alter(&$cache) {
  // Make our handler the one to use for handling user arguments.
  $cache['users']['uid']['argument']['handler'] = 'me_views_handler_argument_user_uid';
}

/**
 * Implementation of hook_views_handlers().
 */
function me_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'me') .'/includes',
    ),
    'handlers' => array(
      'me_views_handler_argument_user_uid' => array(
        'parent' => 'views_handler_argument_user_uid',
      ),
    ),
  );
}
