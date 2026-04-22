<?php
include('inc/session.php');
include('header.php');


if(isset($_SESSION['pornopanel']))
{
	echo '<meta http-equiv="refresh" content="0; URL=index.php">';
}


if(isset($_GET['mode']) && $_GET['mode'] == 'do')
{
	if(isset($_POST['login']))
	{
		if(!empty($_POST['username']) && !empty($_POST['password']))
		{
			$username = $_POST['username'];
			$password = md5($_POST['password']);
					
			if($username == $panel['user'] && $password == $panel['pass'])
			{
				$_SESSION['pornopanel'] = $username;
				echo '<meta http-equiv="refresh" content="0; URL=index.php">';
			}
			else
			{
				echo 'Username/Password incorrect';
			}
			
		} else {
			echo 'Fill in all fields';
		}
	} else {
		echo 'Fill in all fields and press the login-button';
	}
}
else
{
?>
<form method="post" action="login.php?mode=do">
<div>
<table class="loginbox">
<tr><th>Username</th><td><input type="text" name="username" /></td></tr>
<tr><th>Password</th><td><input type="password" name="password" /></td></tr>
<tr><td colspan="2"><input style="width: 100px;" type="submit" value="Login" name="login" /></td></tr>
</table>
</div>
</form>
<?php
}
include('footer.php');
?>