<?php
include('inc/session.php');
if(!isset($_SESSION['pornopanel']))
{
	header("Location: login.php");
}

include('header.php');

if(isset($_GET['mode']) && $_GET['mode'] == 'whitelist')
{
	if(isset($_POST['btn_whitelist']))
	{
		$whitelist = '';
		//echo var_dump($_POST['whitelist']);
		foreach($_POST['whitelist'] as $checking)
		{	
			$whitelist .= $checking.';';
		}
		$whitelist = esc($whitelist);
		
		//echo $whitelist;
		
		$sql_wl = mysql_query("UPDATE settings SET whitelist = '".$whitelist."'");
		if(!$sql_wl)
		{
			echo mysql_errno().' - '.mysql_error();
		}
		
		$tmp_wl = substr($whitelist, 0, -1);
		$wl_parted = explode(';', $tmp_wl);
		
		$blacklist = '';
		
		foreach($ccode as $key => $value)
		{
			if(!in_array($key, $wl_parted))
			{
				$blacklist .= $key.';';
			}
		}
		
		//echo '<br><br><br>'.$blacklist;
		
		$sql_bl = mysql_query("UPDATE settings SET blacklist = '".$blacklist."'");
		if(!$sql_bl)
		{
			echo mysql_errno().' - '.mysql_error();
		}
	
	}
	//echo '<meta http-equiv="refresh" content="0; URL='.$panel['url'].'settings">';
}

?>

<h1>Whitelist</h1>
<div class="left box" style="width: 200px; margin-right: 25px;">
<form method="post" action="settings.php?mode=whitelist">
<select name="whitelist[]" multiple="multiple" size="10">
<?php

$sql = mysql_query("SELECT * FROM settings"); 
$res = mysql_fetch_array($sql);
$tmp = substr($res['whitelist'], 0, -1);
$wl = explode(';', $tmp);

foreach($ccode as $key => $value)
{
	if(in_array($key, $wl))
	{
		echo '<option selected="selected" value="'.$key.'">'.$key.' - '.$value.'</option>';
	}
	else
	{
		echo '<option value="'.$key.'">'.$key.' - '.$value.'</option>';
	}
}

?>
</select>
<input type="submit" class="button" name="btn_whitelist" />
</form>
</div>
<hr class="hidden" />

<?php
include('footer.php');
?>