
<?php
function RC4($key, $data) { //encrypt and decrypt $data with the key in $keyfile with an rc4 algorithm
   $pwd = $key;
   $pwd_length = strlen($pwd);
   $key = array();

   //Key auffüllen
   for ($i = 0; $i < 256; $i++) {
      $key[$i] = ord($pwd{$i % $pwd_length});
      $sub[$i] = $i;
   }

   //Substituion-Box vorbereiten
   for ($i = $x = 0; $i < 256; $i++) {
      $x = ($x + $sub[$i] + $key[$i]) % 256;

      //$sub[$i] swap $sub[$x]
      $temp_swap = $sub[$i];
      $sub[$i] = $sub[$x];
      $sub[$x] = $temp_swap;
   }

   //Text verschlüsseln
   for ($a = $j = $i = 0; $i < strlen($data); $i++) {
      $a = ($a + 1) % 256;
      $j = ($j + $sub[$a]) % 256;

      //$sub[$a] swap $sub[$j]
      $temp = $sub[$a];
      $sub[$a] = $sub[$j];
      $sub[$j] = $temp;

      $k = $sub[(($sub[$a] + $sub[$j]) % 256)];
      $Zcipher = ord($data{$i}) ^ $k;
      $Zcrypt .= chr($Zcipher);
   }
   return $Zcrypt;
}
/*
//Usage:
$text = $_GET['text'];
$pw = $_GET[pw];
//Verschlüsseln
$geheimtext = RC4($pw, $text);
echo "<!!!>".$geheimtext."<!!!2>";
//Entschlüsseln
//$klartext = RC4($pw, $geheimtext);

//echo $klartext;*/
?>