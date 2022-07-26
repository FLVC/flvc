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

  $form['flvc_display_view_for_empty_collections'] = array(
    '#type' => 'checkbox',
    '#title' => t('For anonymous users, show the collection view rather than search results for empty collections'),
    '#description' => t('If checked, empty collections will display the collection view rather than search results.'),
    '#default_value' => variable_get('flvc_display_view_for_empty_collections', FALSE),
  );

  $form['flvc_purl_valid_server_names'] = array(
        '#type' => 'textfield',
        '#title' => t('Valid PURL server names'),
        '#description' => t('Enter valid PURL server names, including legacy (separated by commas).'),
        '#default_value' => variable_get('flvc_purl_valid_server_names', 'purl.flvc.org,purl.fcla.edu'),
    );

    $form['flvc_purl'] = array(
      '#type' => 'fieldset',
      '#title' => t('PURL API Settings'),
      'flvc_purl_enabled' => array(
        '#type' => 'checkbox',
        '#title' => t('Enable use of PURL API'),
        '#default_value' => variable_get('flvc_purl_enabled', FALSE),
      ),
      'flvc_purl_server' => array(
        '#type' => 'textfield',
        '#title' => t('PURL server'),
        '#description' => t('Enter location of the PURL server.'),
        '#default_value' => variable_get('flvc_purl_server', 'https://'),
        '#states' => array(
          'invisible' => array(
            ':input[name="flvc_purl_enabled"]' => array('checked' => FALSE),
          ),
        ),
      ),
      'flvc_purl_apikey' => array(
        '#type' => 'textfield',
        '#title' => t('PURL API key'),
        '#description' => t('Enter the PURL API key.'),
        '#default_value' => variable_get('flvc_purl_apikey', ''),
        '#states' => array(
          'invisible' => array(
            ':input[name="flvc_purl_enabled"]' => array('checked' => FALSE),
          ),
        ),
      ),
      'flvc_purl_target_hostname' => array(
        '#type' => 'textfield',
        '#title' => t('PURL target site name'),
        '#description' => t('Enter the target site name.'),
        '#default_value' => variable_get('flvc_purl_target_hostname', 'https://site.digital.flvc.org'),
        '#states' => array(
          'invisible' => array(
            ':input[name="flvc_purl_enabled"]' => array('checked' => FALSE),
          ),
        ),
      ),
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
  variable_set('flvc_display_view_for_empty_collections', $form_state['values']['flvc_display_view_for_empty_collections']);
  variable_set('flvc_purl_valid_server_names', $form_state['values']['flvc_purl_valid_server_names']);
  variable_set('flvc_purl_enabled', $form_state['values']['flvc_purl_enabled']);
  variable_set('flvc_purl_server', $form_state['values']['flvc_purl_server']);
  variable_set('flvc_purl_apikey', $form_state['values']['flvc_purl_apikey']);
  variable_set('flvc_purl_target_hostname', $form_state['values']['flvc_purl_target_hostname']);

  drupal_set_message('Configuration saved.');
}
