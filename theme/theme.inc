<?php

/**
 * @file
 * Theme hooks.
 */

/**
 * Implements hook_preprocess_theme().
 */
function flvc_preprocess_flvc_toc(array &$variables) {
  $object = $variables['object'];
  $toc_from_datastream = array();

  if (isset($object['TOC']))
  {
    $toc_from_datastream = drupal_json_decode($object['TOC']->content);
  }
  $variables['toc_entries'] = $toc_from_datastream;
  $module_path = drupal_get_path('module', 'flvc');
  $library_path = libraries_get_path('jstree-v.pre1.0');
  drupal_add_js("$library_path/_lib/jquery.js");
  drupal_add_js("$library_path/_lib/jquery.cookie.js");
  drupal_add_js("$library_path/jquery.jstree.js");
  drupal_add_js("$module_path/js/toc-tree.js");
}
