<?php
// $Id$
/**
 * @file
 * Provide user name argument handler.
 */

/**
 * Argument handler to accept a user id.
 */
class me_views_handler_argument_user_name extends views_handler_argument_string {
  /**
   * Information about options for all kinds of purposes will be held here.
   * @code
   * 'option_name' => array(
   *  - 'default' => default value,
   *  - 'translatable' => TRUE/FALSE (wrap in t() on export if true),
   *  - 'contains' => array of items this contains, with its own defaults, etc.
   *      If contains is set, the default will be ignored and assumed to
   *      be array()
   *
   *  ),
   *  @endcode
   * Each option may have any of the following functions:
   *  - export_option_OPTIONNAME -- Special export handling if necessary.
   *  - translate_option_OPTIONNAME -- Special handling for translating data
   *    within the option, if necessary.
   */
  function option_definition() {
    $options = parent::option_definition();

    $options['me_alias'] = array('default' => FALSE);
    $options['me_redirect'] = array('default' => FALSE);
    return $options;
  }

  /**
   * Present options for the user.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // Allow the view creator to decide how the 'me' alias will be handled.
    $form['me_alias'] = array(
      '#type' => 'checkbox',
      '#title' => t("Let users enter the 'me' alias instead of their user name."),
      '#description' => t("If selected, users can enter the 'me' alias inplace of their user name. When this option is selected, the wildcard can not be the same as the 'me' alias and you will not be able to select a user whos name is the same as the 'me' alias."),
      '#default_value' => !empty($this->options['me_alias']),
    );

    // Allow the view creator to decide if the 'me' alias will redirect to the name.
    $form['me_redirect'] = array(
      '#type' => 'checkbox',
      '#title' => t("Redirect to the users name when 'me' is entered as an argument."),
      '#description' => t("If selected, when a user enters 'me' for this argument, they will be redirected to the view with their user name inplace of 'me'."),
      '#default_value' => !empty($this->options['me_redirect']),
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-me-alias' => array(TRUE)),
    );
  }

  /**
   * Validate the options form.
   */
  function options_validate($form, &$form_state) {
    parent::options_validate($form, $form_state);

    // Make sure the wildcard is not the same as the 'me' alias.
    if (!empty($form_state['values']['options']['me_alias']) && _me_is_alias($form_state['values']['options']['wildcard'])) {
      form_set_error('wildcard', t("When using the 'me' alias option, the wildcard can not be the same as the 'me' alias."));
    }
  }

  /**
   * Set the input for this argument
   *
   * @return TRUE if it successfully validates; FALSE if it does not.
   */
  function set_argument($arg) {
    // Only modify the argument when the wildcard does not equal the 'me' alias.
    // Whilst we do validate this in the options form, this is here for views that
    // were created before the me module was installed.
    if (!_me_is_alias($this->options['wildcard']) && !empty($this->options['me_alias'])) {
      // Set a flag on the view so we know if we need to redirect or not.
      $this->view->me_redirect = $this->options['me_redirect'];
      $arg = _me_views_set_argument($arg, $this->view, $this->options['break_phrase'], TRUE);

      // If we are redirecting, make sure the argument does not get validated on the first pass,
      // as this can cause multiple messages or errors, or the redirect itself not to happen.
      // The validation will happen after the redirect. This is only an issue for the validation,
      // as nothing else oon the argument can cause the same amount of grief.
      if (!empty($this->view->me_redirect)) {
        $this->argument_validated = TRUE;
      }
    }

    // We always need to return the parent::set_argument() call.
    return parent::set_argument($arg);
  }
}

