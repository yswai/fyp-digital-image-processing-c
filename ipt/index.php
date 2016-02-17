<?php
	session_start();
	
	if(isset($_POST['imagefile']))
	{
		$tmp = $_POST['imagefile'];
		$_SESSION['imgf'] = $tmp;
	} else {
		if(isset($_SESSION['imgf']))
		{
			$_SESSION['imgf'] = $_SESSION['imgf'];
		} else {
			$_SESSION['imgf'] = 'flower.png';
		}
	}

	$inputImage = $_SESSION['imgf'];
	$ip = $_POST['function'];
	$contrastc_value = $_POST['contrastc_value'];
	$brightness_value = $_POST['brightness_value'];
	$outputImage = "out_".$inputImage;
	$prog = "/root/PNG/image_processing_toolbox ";
	$imagePath = "/usr/share/images/";	
	
	if(isset($_POST['imagefile'])) 
	{
	$cmmnd = 'cp '.$imagePath.$inputImage.' '.$imagePath.$outputImage;
	`$cmmnd`;
	}	


	// text box handler

	if(isset($contrastc_value))
	{
		$cmmnd = $prog.'-contrastc '.$contrastc_value.' '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif(isset($brightness_value))
	{
		$cmmnd = $prog.'-brightness '.$brightness_value.' '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	}


	// buttons handler
	
	if($ip == 'dilation')
	{
		$cmmnd=$prog.'-dilation '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'erosion')
	{
		$cmmnd = $prog.'-erosion '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'histeq')
	{
		$cmmnd = $prog.'-histeq '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput =`$cmmnd`;
	} elseif($ip == 'sharpen')
	{
		$cmmnd = $prog.'-sharpen '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'stdavg')
	{
		$cmmnd = $prog.'-stdavg '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'weightedavg')
	{
		$cmmnd = $prog.'-weightedavg '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'gaussianblur')
	{
		$cmmnd = $prog.'-gaussianblur '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'laplacian')
	{
		$cmmnd = $prog.'-laplacian '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'linedet')
	{
		$cmmnd = $prog.'-linedet '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'edgedet')
	{
		$cmmnd = $prog.'-edgedet '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'rgb2gray')
	{
		$cmmnd = $prog.'-rgb2gray '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'gradient_n')
	{
		$cmmnd = $prog.'-gradient '.'n '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'gradient_w')
	{
		$cmmnd = $prog.'-gradient '.'w '.$imagePath.$inputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'gradient_s')
	{
		$cmmnd = $prog.'-gradient '.'s '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'gradient_e')
	{
		$cmmnd = $prog.'-gradient '.'e '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'prewitt_h')
	{
		$cmmnd = $prog.'-prewitt '.'h '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'prewitt_v')
	{
		$cmmnd = $prog.'-prewitt '.'v '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'contrasts')
	{
		$cmmnd = $prog.'-contrasts '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'fliph')
	{
		$cmmnd = $prog.'-fliph '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'flipv')
	{
		$cmmnd = $prog.'-flipv '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'opening')
	{
		$cmmnd = $prog.'-opening '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'closing')
	{
		$cmmnd = $prog.'-closing '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'weightedmean')
	{
		$cmmnd = $prog.'-weightedmean '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'weightedmedian')
	{
		$cmmnd = $prog.'-weightedmedian '.$imagePath.$outputImage.' '.$imagePath.$outputImage;
		$mainOutput = `$cmmnd`;
	} elseif($ip == 'rst')
	{
		$cmmnd = 'cp '.$imagePath.$inputImage.' '.$imagePath.$outputImage;	
		$mainOutput = `$cmmnd`;
	} 
	
	
?>


<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
"http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
	<title> Embedded Digital Image Processing Toolbox</title>
	<script type="text/javascript" src="jquery.js"></script>
	<script type="text/javascript" src="custom.js"></script>
	
	<style type="text/css">

div.cmdoutput
{
min-width:1024px;
width:95%;
background-color:#000000;
margin-left:5px;
text-align:left;
font-family:courier;
color:#0FFF00;
border:double 8px white;
}		
div.menu-top, input.common, button.cmd_toggle
{
font-family: "Trebuchet MS", Verdana, Helvetica, Arial,sans-serif !important;
margin:0;
padding:0;
letter-spacing:0;
}
h1.common
{
font-family: "Trebuchet MS", Verdana, Helvetica, Arial,sans-serif;
size:0.9em;
color:#EEE;
text-shadow:#0066CC 2px 2px 2px;
margin-top:0;
margin:0;
padding:0;
}
.type1
{
width:250px;
font-size:1.05em;
color:#FFF;
text-shadow:#000 1px 1px 1px;
font-family: "Trebuchet MS", Verdana, Helvetica, Arial, sans-serif !important;
margin-left:10px;
margin-top:3px;
margin-bottom:3px;
border-top-style:none;
border-bottom-style:groove;
border-left-style:none;
border-right-style:groove;
}
button.type2
{
text-align:left;
font-size:1.0em;
width:150px;
color:#FFF;
text-shadow:#000 1px 1px 1px;
font-family: "Trebuchet MS", Verdana, Helvetica, Arial, sans-serif !important;
margin-left:20px;
margin-top:0px;
margin-bottom:2px;
border-top-style:none;
border-bottom-style:groove;
border-left-style:none;
border-right-style:groove;
}
h3.imgOps
{
color:#FFFFFF;
text-shadow:#000 2px 2px 2px;
background-color:#666666;
border:solid 2px #000000;
width:300px;
padding-left:10px;
margin:0;
}
h1.header
{
min-width:800px;
margin-bottom:0px;
color:#EEE;
text-shadow:#0066CC 3px 3px 3px;
text-align:center;
font-family: "Trebuchet MS", Verdana, Helvetica, Arial, sans-serif !important;
font-size:2.5em;
height:100px;
background-image:url(./images/header.gif);
background-repeat:repeat-x;
}
h5.small
{
margin-top:0;
margin-bottom:0px;
padding:0;
text-align:right;
font-size:0.57em;
font-family: "Trebuchet MS", Verdana, Helvetica, Courier, sans-seri;
}
h5.footer
{
margin-bottom:10px;
padding:0;
text-align:center;
font-family: "Trebuchet MS", Verdana, Helvetica, Courier, sans-seri;
font-size:0.7em;
}

</style>

</head>
<body style="background-image:url(./images/body-bg.jpg);background-repeat:no-repeat;background-position:bottom left">
<h1 class="header">Embedded Digital Image Processing Toolbox</h1>
<h5 class="small">Developed-by <b>Yep Siong Wai</b><br/> Bachelor of Computer Engineering, University Malaysia Perlis (UniMAP).</h5>
<h5 class="small">Powered-by TS-7800 SBC</h5>
<hr style="width:98%;border:none 0;border-top:1px solid #ccc;border-bottom:1px solid #efefef:margin-top:0px;"></hr>
<br/>
<div class="cmdoutput" >
	<p style="margin-left:5px;">
		<br/>
		Using Image : <?php echo $inputImage; ?> 
		<br/>
		<br/>
		Command Invoked:
		<br/>
		<?php print $cmmnd;?>
		<br/>
		<br/>
		Output Log: 
		<br/>
		<?php print nl2br($mainOutput);?>
		<br/>
	</p>
</div>
<button type="button" class="type1" id="cmd_toggle">Hide Terminal Output</button>
<hr style="width:98%;border:none 0;border-top:1px solid #ccc;border-bottom:1px solid #efefef;text-align:center"></hr>
<br/>

<br/>
<br/>

<table style="max-width:1024px;width:1024px">	
<tr>
<td style="width:400px;background-image:url(./images/menu-bg.jpg);background-repeat:no-repeat">
<h1 class="common">Image Processing Operation:</h1>
<br/>
	<div class="menu-top">
	<form style="width:90%;align:left" action="index.php" method="POST">
		<h3 class="imgOps" id="spatialf"> Spatial Filtering >> </h3>
			<div style="margin-left:10px" id="spatialf">
				<button class="type1" name="function" value="gaussianblur">Gaussian Blur</button>
				<br/>
				<button class="type1" name="function" value="laplacian">Laplacian Filter</button>
				<br/>
				<button class="type1" name="function" value="weightedavg">Smoothing (Weighted Averaging)</button>
				<br/>
				<button class="type1" name="function" value="stdavg">Smoothing (Standard Averaging)</button>
				<br/>
				<button class="type1" name="function" value="sharpen">Sharpening Mask (High Pass Spatial Filter)</button>
				<br/>
				<button class="type1" name="function" value="weightedmean">Weighted Mean Filter</button>
				<br/>
				<button class="type1" name="function" value="weightedmedian">Weighted Median Filter</button>
				<br/>
				<button class="type1" name="function" value="edgedet">Edge Detection Filter</button>
				<br/>
				<button disabled class="type1" id="prewitt">Prewitt's Mask >> </button>
					<div id="prewitt" style="margin-left:10px;">
					<button class="type2" name="function" value="prewitt_h">>> Horizontal </button>
					<br/>
					<button class="type2" name="function" value="prewitt_v">>> Vertical </button>
					<br/>
					</div>	
				<button disabled class="type1" id="gradient">Gradient Masks >> </button>
					<div id="gradient" style="margin-left:10px">
					<button class="type2" name="function" value="gradient_n">>> North </button>
					<br/>
					<button class="type2" name="function" value="gradient_e">>> East</button>
					<br/>
					<button class="type2" name="function" value="gradient_s">>> South </button>
					<br/>
					<button class="type2" name="function" value="gradient_w">>> West </button>
					<br/>
					</div>
			</div>
		<br/>

		<h3 class="imgOps" id="morphology">Morphological Filter Class >> </h3>
			<div style="margin-left:10px" id="morphology">
				<button class="type1" name="function" value="dilation">Dilation (5x5 Rectangular)</button>
				<br/>
				<button class="type1" name="function" value="erosion">Erosion (5x5 Rectangular)</button>	
				<br/>
				<button class="type1" name="function" value="closing">Closing</button>
				<br/>
				<button class="type1" name="function" value="opening">Opening</button>	
				<br/>
			</div>
		<br/>

		<h3 class="imgOps" id="histogram">Histogram Based Processing >> </h3>
			<div style="margin-left:10px" id="histogram">
				<button class="type1" name="function" value="histeq">Histogram Equalization</button>
				<br/>
				<button class="type1" name="function" value="contrasts">Contrast Stretching</button>
	</form>
				<br/>
	<form style="width:90%;align:left" action="index.php" method="POST">
				<button disabled class="type1" id="contrasts">Contrast Correction >></button>
					<div id="contrastc" style="margin-left:10px">
					Contrast Modifier:
					<br/><input type="text" name="contrastc_value" value="[-100 to 100]">
					<input type="submit" value="Go!">
					<br/>
					</div>	
	</form>

	<form style="width:90%;align:left" action="index.php" method="POST">
				<button disabled class="type1" id="brightness">Brightness Adjustment >></button>
					<div id="brightness" style="margin-left:10px">
					Brightness Modifier:
					<br/><input type="text" name="brightness_value" value="[-100 to 100]">
					<input type="submit" value="Go!">
					<br/>
					</div>
	</form>
			</div>
		<br/>
		
	<form style="width:90%;align:left" action="index.php" method="POST">
		<h3 class="imgOps" id="others">Others >> </h3>
			<div style="margin-left:10px" id="others">
				<button class="type1" name="function" value="fliph">Flip Horizontal</button>
				<br/>
				<button class="type1" name="function" value="flipv">Flip Vertical</button>
				<br/>
				<button class="type1" name="function" value="rgb2gray">Color Image (RGB) to Graylevel (8-bit)</button>	
				<br/>
			</div>
		<br/>


		<!--
		<h3 class="imgOps" id="noise">Add Noise >> </h3>
			<div style="margin-left:10px" id="noise">
				<button class="type1" name="function" value="gnoise">Gaussian Noise</button>
				<br/>
				<button class="type1" name="function" value="unoise">Uniform Noise</button>
				<br/>
			</div>
		-->


		<br/>
		<br/>
		<h1 class="common">File/Directory Operation:</h1>
		<br/>
	</form>
		<br/>
		<h3 class="imgOps" id="file_listing">List/Change Image Files >> </h3>
		<div id="file_listing">
		<?php echo nl2br(`ls /usr/share/images`); ?>
		<br/>
		Please enter image file to be used: <br/>
	<form method="POST" action="index.php">
		<input type="text" name="imagefile"><input type="submit" value="Go!">
		</div>
	</form>
		<br/>
		<h3 class="imgOps" id="upload">Upload PNG Image >> </h3>
		<br/>
		<div id="upload">
			<form enctype="multipart/form-data" action="uploader.php" method="POST">
			<input type="hidden" name="MAX_FILE_SIZE" value="1000000" />
			&nbsp;Choose a file to upload: <input name="uploadedfile" type="file" /><br />
			<input type="submit" class="type1" value="Upload File" />
			</form>
		</div>
		<br/>
		<br/>
		<hr style="border:1px solid #efefef"/>PNG Image
	<form method="POST" action="index.php">
		<button class="type1" name="function" value="rst">Reset Image</button>
		<br/>
	</form>
	</div>
</td>
<td style="text-align:center;border-left:1px solid #efefef;padding:5px;">
Input Image:
<br/>
<img src="/images/<?php print $inputImage ?>">
<br/>
Output Image:
<br/>
<img src="/images/<?php print $outputImage?>">
</td>
</table>

<br/>
<br/>
<hr style="width:95%;border:none 0;border-top:1px solid #ccc;border-bottom:2px solid #efefef"></hr>
<h5 class="footer">&copy; 2009-2010 | Yep Siong Wai | yswai1986@gmail.com</h5> 
</body>
</html>


