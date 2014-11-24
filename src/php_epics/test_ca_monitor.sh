#!/usr/bin/php -q
<?PHP
dl("php_epics.so");

function chanChanged($chan, $value) {
	echo "$chan $value\n";
	flush();
}

ob_end_flush();

echo "Doing ca_monitor...\n";
$rt=ca_monitor("TEST:ai1,TEST:msg1", 40, "chanChanged");
echo "\nDone.\n";
?>
