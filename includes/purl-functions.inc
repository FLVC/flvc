<?php

function purl_process($purl, $owning_inst ,$pid) {

  //$config = parse_ini_file("config.ini");

  //$purl_server = $config['purl_server'];
  //$purl_api_key = $config['purl_api_key'];
  //$target_ext = $config['target_extension'];
  //$owning_inst_map = $config['owning_inst_map'];

  $purl_server = variable_get('flvc_purl_server', '');
  $purl_api_key = variable_get('flvc_purl_apikey', '');
  $purl_target_hostname = variable_get('flvc_purl_target_hostname', '');
  $mode = "create";

  $purl_api = $purl_server . "/api/purl/";
  $type = "302";

  //$target = "https://";
  //if (array_key_exists($owning_inst, $config['owning_inst_map'])) {
  //  $target .= $config['owning_inst_map'][$owning_inst];
  //}
  //else {
  //  $target .=  $owning_inst;
  //}
  $target = $purl_target_hostname . "/islandora/object/" . urlencode($pid);

  $outstream =  fopen("php://stderr", "a");

  if ($mode == "create") {
    $purl_id = purl_get($purl_server, $purl_api_key, $purl, $outstream);
    if ($purl_id == "0") {
      purl_create($purl_api, $purl_api_key, $purl, $type, $target, $owning_inst, $outstream);
    }
    else {
      $url = $purl_api . $purl_id;
      purl_update($url, $purl_api_key, $purl, $type, $target, $owning_inst, $outstream);
    }
  }
  else if ($mode == "delete") {
    $purl_id = purl_get($purl_server, $purl_api_key, $purl, $outstream);
    if ($purl_id == "0") {
      fwrite($outstream, "purl not found - cannot be deleted\n");
    }
    else {
      $url = $purl_api . $purl_id;
      purl_delete($url, $purl_api_key, $outstream);
    }
  }

  fclose($outstream);

}

function build_header($purl_api_key) {
  $header_values = array(
        'Content-type' => 'application/json',
        'KiwiApiKey' => $purl_api_key
  );
  array_walk($header_values, static function(&$v, $k) { $v = $k.': '.$v; });
  $header = implode("\r\n", $header_values);
  return $header;
}

function get_http_code($header) {
  preg_match('{HTTP\/\S*\s(\d{3})}', $header, $match);
  if (count($match) == 2) {
    return $match[1];
  }
  return "0";
}

function purl_get($url, $purl_api_key, $purl, $outstream) {
  $purl_id = "0";
  $admin_url = $url . '/admin/purl' . $purl;
  $result = @file_get_contents($admin_url);
  if ($result) {
    $purl = json_decode($result);
    fwrite($outstream, "purl found = " . $purl->purlId . "\n");
    $purl_id = $purl->purlId;
  }
  else {
    fwrite($outstream, "purl not found: " . $admin_url . "\n");
  }
  return $purl_id;
}

function purl_create($url, $purl_api_key, $purl, $type, $target, $owning_inst, $outstream) {
  fwrite($outstream, "creating purl\n");
  $data = array('purlPath' => $purl, 'type' => $type, 'target' => $target, 'institutionCode' => $owning_inst);
  // use key 'http' even if you send the request to https://...
  $options = array(
    'http' => array(
        'header'  => build_header($purl_api_key),
        'method'  => 'POST',
        'content' => json_encode($data, JSON_UNESCAPED_SLASHES),
        'ignore_errors' => true
    )
  );
  $context  = stream_context_create($options);
  $result = file_get_contents($url, false, $context);
  $status = get_http_code($http_response_header[0]);
  if ($status != "201") {
    fwrite($outstream, "ERROR " . $http_response_header[0] . ": ");
  }

  fwrite($outstream, $result . "\n");
}

function purl_update($url, $purl_api_key, $purl, $type, $target, $owning_inst, $outstream) {
  fwrite($outstream, "updating purl\n");
  $data = array('purlPath' => $purl, 'type' => $type, 'target' => $target, 'institutionCode' => $owning_inst);

  $options = array(
    'http' => array(
        'header'  => build_header($purl_api_key),
        'method'  => 'PUT',
        'content' => json_encode($data, JSON_UNESCAPED_SLASHES),
        'ignore_errors' => true
    )
  );
  $context  = stream_context_create($options);
  $result = file_get_contents($url, false, $context);
  $status = get_http_code($http_response_header[0]);
  if ($status != "200") {
    fwrite($outstream, "ERROR " . $http_response_header[0] . ": ");
  }

  fwrite($outstream, $result . "\n");
}

function purl_delete($url, $purl_api_key, $outstream) {
  fwrite($outstream, "tombstoning purl\n");

  $options = array(
    'http' => array(
        'header'  => build_header($purl_api_key),
        'method'  => 'DELETE',
        'ignore_errors' => true
    )
  );
  $context  = stream_context_create($options);
  $result = file_get_contents($url, false, $context);
  $status = get_http_code($http_response_header[0]);
  if ($status != "200") {
    fwrite($outstream, "ERROR " . $http_response_header[0] . ": ");
  }

  fwrite($outstream, $result . "\n");
}

?> 
