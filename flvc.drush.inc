<?php

/**
 * @file
 * Implementation of Drush hooks.
 */

/**
 * Implements hook_drush_command().
 */
function flvc_drush_command() {
  $items = array();

  $items['flvc_remove_bad_inherits'] = array(
    'description' => 'Remove bad inherit relationships.',
    'drupal dependencies' => array('islandora_xacml_editor'),
  );

  $items['flvc_change_parent'] = array(
    'description' => 'Change the parent relationship of an object.',
    'drupal dependencies' => array('islandora'),
    'options' => array(
      'object' => array(
        'description' => 'The PID of the object to change.',
        'required' => TRUE,
      ),
      'parent' => array(
        'description' => 'The PID of the parent object.',
        'required' => TRUE,
      ),
      'parent_relationship_pred' => array(
        'description' => 'The predicate of the relationship to the parent. ' .
        'Defaults to "isMemberOf".',
        'value' => 'optional',
      ),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );

  return $items;
}

/**
 * Implements hook_drush_command().
 */
function drush_flvc_remove_bad_inherits() {

  drupal_load('module', 'islandora');

  // XXX: We want to force things to be done as admin, and running the update
  // through drush seems to break things, despite specifying user 1... Let's
  // force things.
  drupal_static_reset('islandora_get_tuque_connection');
  $user = user_load(1);
  $tuque = islandora_get_tuque_connection($user);

  //$tuque = islandora_get_tuque_connection();
  $pred = 'inheritXacmlFrom';
  $rels_uri = ISLANDORA_RELS_EXT_URI;

  $query = <<<EOQ
PREFIX islandora: <$rels_uri>
SELECT DISTINCT ?object
FROM <#ri>
WHERE {
  ?object islandora:$pred <info:fedora/>
}

EOQ;

  $total = $tuque->repository->ri->countQuery($query, 'sparql');
  if ($total == 0) {
    // Nothing to process.
    return t('Nothing to fix.');
  }


  $offset = 100;
  $limited_query = $query . <<<EOQ
LIMIT $offset
EOQ;

  $result_stash = $tuque->repository->ri->sparqlQuery($limited_query);
  if (empty($result_stash)) {
      // Ran out of items early?
      return t('No PID(s) found to fix.');
  }

  foreach ($result_stash as $result) {
    //$result = array_pop($sandbox['result_stash']);
    $updating_pid = $result['object']['value'];
    $object_to_update = islandora_object_load($updating_pid);
    if ($object_to_update) {
      $rels = $object_to_update->relationships;
      $xacml_parents = array_merge($rels->get(ISLANDORA_RELS_EXT_URI, $pred), $rels->get(FEDORA_RELS_EXT_URI, $pred));
      $xacml_parent = reset($xacml_parents);
      if ($xacml_parent) {
        $rels->autoCommit = FALSE;
        $rels->remove(FEDORA_RELS_EXT_URI, $pred);
        $rels->remove(ISLANDORA_RELS_EXT_URI, $pred);
        $rels->commitRelationships();
        watchdog('islandora_xacml_api', 'Removed empty "@pred" from @pid.', array(
          '@pred' => $pred,
          '@pid' => $updating_pid,
        ));
      }
      else {
        // Should never be able to get here really... Somebody else is running
        // this at the same time?
        watchdog('islandora_xacml_api', 'Selected @pid to update "@pred" literal, but was missing relationships when we checked!?', array(
          '@pred' => $pred,
          '@pid' => $object_to_update->id,
        ));
      }
    }
    else {
      watchdog('islandora_xacml_api', 'Selected @pid to update "@pred" literal, but object not found!?', array(
        '@pred' => $pred,
        '@pid' => $updating_pid,
      ));
    }
  } //foreach
}

function drush_flvc_change_parent() {
  drupal_load('module', 'islandora');
  $connection = islandora_get_tuque_connection();

  $object_pid = drush_get_option('object');
  $parent_pid = drush_get_option('parent');
  $parent_relationship_pred = drush_get_option('parent_relationship_pred', 'isMemberOf');

  $object = islandora_object_load($object_pid);
  if ($object) {
    $rels = $object->relationships;
    //$parents = $rels->get(FEDORA_RELS_EXT_URI, $parent_relationship_pred);
    //$rels->autoCommit = FALSE;
    $rels->remove(FEDORA_RELS_EXT_URI, $parent_relationship_pred);
    $rels->add(FEDORA_RELS_EXT_URI, $parent_relationship_pred, $parent_pid);
    $rels->commitRelationships();
  }
  else {
        watchdog('flvc', 'No object loaded for @pid.', array(
          '@pid' => $object_pid,
        ));
  }
}
