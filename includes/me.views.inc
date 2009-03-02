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
  $cache['users']['name']['argument']['handler'] = 'me_views_handler_argument_user_name';
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
    'handlers' => array(
      'me_views_handler_argument_user_name' => array(
        'parent' => 'views_handler_argument_string',
      ),
    ),
  );
}

/**
 * Helper function to set the views user arguments we override.
 *
 * @param $arg
 *   The arg(s) we are checking.
 * @param $break_phase
 *   Helps us determine if there are multiple arguments.
 * @param $username
 *   Wehter or not this is the username argument.
 *
 * @return string
 *   The modified argument list.
 */
function _me_views_set_argument($arg, $break_phase, $username = FALSE) {
  $uid_args = array();
  $seperator = ' ';
  if (empty($break_phase)) {
    $uid_args[] = $arg;
  }
  else {
    // Modified from views_break_phrase() to include characters that a 'me' alias
    // may include.
    if (preg_match('/^([0-9a-zA-Z]+[+ ])+[0-9a-zA-Z]+$/', $arg)) {
      // The '+' character in a query string may be parsed as ' '.
      $uid_args = preg_split('/[+ ]/', $arg);
    }
    else if (preg_match('/^([0-9a-zA-Z]+,)*[0-9a-zA-Z]+$/', $arg)) {
      $seperator = ',';
      $uid_args = explode(',', $arg);
    }
  }

  // The alias could potentially show up more than once. Loop over each argument
  // and check to be sure.
  foreach ($uid_args as &$uid_arg) {
    $uid_arg = _me_check_arg($uid_arg, $username, FALSE);
  }

  return implode($seperator, $uid_args);
}
