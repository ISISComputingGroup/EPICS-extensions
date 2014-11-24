#!/usr/bin/php -q
<?PHP
dl("php_epics.so");
echo "Doing ca_get...\n";
echo ca_type("TEST:ai1")."\n";
$rt=ca_get("TEST:ai1");
echo "\nTEST:ai1 ".$rt."\n";

#echo "Doing ca_put...\n";
#$rt=ca_put("TEST:ai1","13.3");
#echo "Doing ca_get...\n";
#$rt=ca_get("TEST:ai1");
#echo "\nTEST:ai1 ".$rt."\n";
?>
