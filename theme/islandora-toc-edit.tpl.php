<?php
/**
 * @file
 *
 */
?>
<?php if (isset($toc_entries['title'])): ?>
  <?php $booktitle = $toc_entries['title']; ?>
  <?php print "<h3>$booktitle</h3>"; ?>
<?php endif; ?>
<h4>Table of Contents</h4>
<form>
<textarea rows="30" cols="100">
<?php print $toc_string ?>
</textarea>
<input type="submit" value="Update TOC datastream">
</form>
