<?php
// $Id$

/**
 * @file
 *
 * Provides views intergration for the me module.
 */

/**
 * Implementation of hook_views_pre_view().
 */
function me_views_pre_view(&$view, &$display_id, &$args) {
  // Loop over each of the defined arguments. If the argument is uid,
  // then we need to check that argument for 'me' and replace it with the
  // current user id.
  // This could potentially break wildcard arguments that are 'me'. So bascially do not
  // create a wildcard 'me' for the User:UID option in the views UI.
  $arguments = $view->display[$display_id]->handler->default_display->options['arguments'];
  for ($i = 0, list($type, $data) = each($arguments); $type && $data; list($type, $data) = each($arguments), $i++) {
    if ($type == 'uid' && !is_null($args[$i])) {
      $uid_args = explode(' ', $args[$i]);

      // The alias could potentially show up more than once. Loop over each argument
      // and check to be sure.
      foreach ($uid_args as &$arg) {
        $arg = _me_check_arg($arg);
      }

      $args[$i] = implode(' ', $uid_args);
      break;
    }
  }
}
