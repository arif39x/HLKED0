<?php
	/* Copyright © 2012 by Chrystal */
	
	if (!defined('__CP__')) { die(); }
	
	class Base
	{
		public function VisualDecrypt($Data)
		{
            for ($a = (strlen($Data) - 1); $a > 0; $a--)
                $Data[$a] = chr(ord($Data[$a]) ^ ord($Data[($a - 1)]));
			
			return $Data;
		}
		
		public function GetCountryName($Code)
		{
			$GI = new GeoIP;
			return $GI->GEOIP_COUNTRY_NAMES[$GI->GEOIP_COUNTRY_CODE_TO_NUMBER[$Code]];
		}
		
		public function Truncate($Data, $Length)
		{
			if (strlen($Data) >= $Length)
				{ return (substr($Data, 0, $Length) . ' [...]'); }
					else { return $Data; }
		}
	}
?>