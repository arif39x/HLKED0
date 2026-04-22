<?php include('auth.php'); ?>
<html>
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
    <title>Umbra Loader</title>
    <link rel="stylesheet" type="text/css" href="../resources/css/ext-all.css" />
 	<script type="text/javascript" src="../adapter/ext/ext-base.js"></script>
    <script type="text/javascript" src="../ext-all.js"></script>
    <style type="text/css">
        body
        {
            font-family: Arial, Verdana, sans-serif;
            background-image: url(graphics/background_gradient.png);
            background-repeat: repeat-x;
            background-color: #374d5b;
            margin: 0;
            padding: 0 0 20px 0;
            font-size: 13px;
            color: #333;
            text-align: center;
        }
        .refreshicon 
        {
            background-image: url( graphics/refresh.png ) !important;
        }
        .root
        {
            width: 410px;
            margin: 0 auto;
            text-align: left;
            background-color: #e6e6e6;
            position: relative;
        }

        #header
        {
            background: #0e1d2f url(graphics/banner.png) no-repeat left;
            height: 90px;
        }
    </style> 
</head>
<body>
    <div class="root">
        <div id="header"></div>
    </div>
    <script type="text/javascript" src="list-view.js"></script>
</body>
</html>
