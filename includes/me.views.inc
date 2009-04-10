<?php
// $Id$

/**
 * @file
 *
 * Provides views intergration for the me module.
 */

/**
 * Implementation of views' hook_views_data_alter().
 *
 * See http://drupal.org/project/views for module and hook information.
 */
function me_views_data_alter(&$cache) {
  // Make our handler the one to use for handling user arguments.
  // We need to override the file, path, and parent here so views with one
  // of the user id or username arguments don't break. Because we are doing an
  // override, we actually don't need to implement hook_views_handlers().

  // UID argument.
  $cache['users']['uid']['argument']['handler'] = 'me_views_handler_argument_user_uid';
  $cache['users']['uid']['argument']['file'] = 'me_views_handler_argument_user_uid.inc';
  $cache['users']['uid']['argument']['path'] = drupal_get_path('module', 'me') .'/includes';
  $cache['users']['uid']['argument']['parent'] = 'views_handler_argument_user_uid';

  // Name argument.
  $cache['users']['name']['argument']['handler'] = 'me_views_handler_argument_user_name';
  $cache['users']['name']['argument']['file'] = 'me_views_handler_argument_user_name.inc';
  $cache['users']['name']['argument']['path'] = drupal_get_path('module', 'me') .'/includes';
  $cache['users']['name']['argument']['parent'] = 'views_handler_argument_string';
}

/**
 * Implementation of Views' hook_views_pre_execute().
 *
 * See http://drupal.org/project/views for module and hook information.
 */
function me_views_pre_execute(&$view) {
  // Check if we need to do a redirect.
  if (!empty($view->me_redirect)) {
    // We should always be able to redirect here regardless, as our handler has to
    // have run for our option to be set, which means we need to redirect anyway.

    // Loop over the argument handlers to get the arguments we need. We also keep this
    // consistent with any extra arguments that may have been passed in.
    $arguments = $view->args;
    foreach (array_values($view->argument) as $key => $argument) {
      if (isset($argument->argument))  {
        $arguments[$key] = $argument->argument;
      }
    }

    // Redirect to the path.
    drupal_goto($view->get_url($arguments));
  }
}

/**
 * Helper function to set the views user arguments we override.
 *
 * @param $arg
 *   The arg(s) we are checking.
 * @param &$view
 *   The view object.
 * @param $break_phase
 *   Helps us determine if there are multiple arguments.
 * @param $username
 *   Wehter or not this is the username argument.
 *
 * @return string
 *   The modified argument list.
 */
function _me_views_set_argument($arg, &$view, $break_phase, $username = FALSE) {
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

  // Be sure not to redirect in a live preview.
  if (!empty($view->live_preview)) {
    $view->me_redirect = FALSE;
  }

  // Check if we need to do a redirect, and make sure the option is disabled if we don't.
  if ($view->me_redirect) {
    $redirect_args = array_filter($uid_args, create_function('$n', 'return _me_is_alias($n);'));
    if (empty($redirect_args)) {
      $view->me_redirect = FALSE;
    }
  }

  // The alias could potentially show up more than once. Loop over each argument
  // and check to be sure.
  foreach ($uid_args as $key => $uid_arg) {
    $uid_args[$key] = _me_check_arg($uid_arg, $username, FALSE);
  }

  return implode($seperator, $uid_args);
}
