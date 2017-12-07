<?php

/**
 * @file
 * Administration page callbacks for the FLVC module.
 */

/**
 * FLVC module administration and configuration form.
 *
 * @ingroup forms
 *
 * @return array
 *   The Drupal form definition.
 */
function flvc_admin_form() {
  $form = array();

  $form['flvc_include_collections_in_search'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include collection objects in search results'),
    '#description' => t('If checked, collection objects will appear in Solr search results.'),
    '#default_value' => variable_get('flvc_include_collections_in_search', FALSE),
  );

  $form['save_configuration'] = array(
    '#type' => 'submit',
    '#name' => 'saveConfiguration',
    '#default_value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Submit handler for the admin form for the FLVC IR Custom module.
 *
 * @param array $form
 *   An array representing a form in Drupal.
 * @param array $form_state
 *   An array containing the Drupal form state.
 */
function flvc_admin_form_submit($form, &$form_state) {
    variable_set('flvc_include_collections_in_search', $form_state['values']['flvc_include_collections_in_search']);

    drupal_set_message('Configuration saved.');
}
