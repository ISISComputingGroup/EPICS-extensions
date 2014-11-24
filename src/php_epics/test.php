<?PHP
dl("php_epics.so"); 

function test($a,$b)
{
	echo "I'm called ! $a $b ".time()."<BR>\n";
    flush();
}

ob_end_flush() ;
$res=ca_monitor("TEST-VME-T4_tes:ai001,TEST-VME-T4_tes:bi001",40,"test");

echo "$res... Done.... ".time();

exit;

echo "<HTML><HEAD><TITLE>Epics CA_GET TEST</TITLE></HEAD>\n";
echo "<BODY BGCOLOR=#FFFFFF>\n";
echo "<FORM METHOD=GET ACTION=test.php>\n";
echo "Enter the channel name: <INPUT TYPE=TEXT NAME=CHANNEL VALUE=\"".$_GET["CHANNEL"]."\"><BR>\n";
echo "<INPUT TYPE=SUBMIT VALUE=Go>\n";

echo "<BR><BR>\n";

echo ca_get($_GET["CHANNEL"])."<BR>\n";
ca_put($_GET["CHANNEL"],time());
echo ca_get($_GET["CHANNEL"])."<BR>\n"; 

/* echo "<INPUT TYPE=TEXT NAME=FIELD>\n";
ob_end_flush() ;
if($_GET["CHANNEL"] <> "")
{
	for($i=0;$i < 1000;$i++)
  {
  	//echo "<BR><HR>\n";
  	$return = ca_get($_GET["CHANNEL"]);
    //echo $_GET["CHANNEL"]." = '$return'<BR>\n";
    echo "<SCRIPT>\n";
    echo "document.forms[0].FIELD.value='$return';\n";
    echo "</SCRIPT>\n";
    flush();
    usleep(500000);
    //sleep(1);
  }
}*/
?>
