<!DOCTYPE HTML>
<html>
<head>
<meta charset="utf-8">
<title>Unbenanntes Dokument</title>
</head>

<body>
<?php
/**
 * GeoIP City Database Example
 *
 * @version $Id$
 * @package geoip
 * @copyright © 2006 Lampix.net
 * @author Dragan Dinic <dinke@lampix.net>
 */

require_once("geoipcity.inc");

$gi = geoip_open("GeoLiteCity.dat", GEOIP_STANDARD);

//$ip = $_SERVER['REMOTE_ADDR'];
//if you test on localhost use IP bellow for test
//since $_SERVER['REMOTE_ADDR'] would be 127.0.0.1
$ip = "85.17.97.27";

$record = geoip_record_by_addr($gi, $ip);

if(!$record)
{
	echo "Sorry, we weren't able to locate you.";
}
else
{
	echo "Country: " .$record->country_name . "<br />";
	echo "Country Code: " . $record->country_code . "<br />";
	echo "Country Code 2: " . $record->country_code3 . "<br />";
	echo "Region: " .$record->region . "<br />";
	echo "City: " .$record->city . "<br />";
	echo "Postal Code: " .$record->postal_code . "<br />";
	echo "Latitude: " .$record->latitude . "<br />";
	echo "Longitude: " .$record->longitude . "<br />";
	echo "DMA Code: " .$record->dma_code . "<br />";
	echo "Area Code: " .$record->area_code . "<br />";
}

geoip_close($gi);
?>
</body>
</html>