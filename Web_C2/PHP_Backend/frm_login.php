<?php /* Copyright © 2012 by Chrystal */ if(!defined('__CP__')) { die(); } ?>

<body onload="document.getElementById('password').focus();" oncontextmenu="return false;">
	<div id="page-wrapper">
		<div id="header"></div>
		<form action="" method="post">
			<div id="login">
				<h2>Login</h2>
				<label for="password">Password:</label>
				<input type="password" id="password" name="password" />
				<input type="submit" class="button" value="Login" />
				<div style="margin:20px;"></div>
			</div>
		</form>
	</div>
</body>