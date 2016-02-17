jQuery(document).ready(function() {
	
	$("div#spatialf").hide();
	$("div#morphology").hide();
	$("div#histogram").hide();
	$("div#graphics").hide();
	$("div#others").hide();
	$("div#noise").hide();
	$("div#file_listing").hide();
	$("div#upload").hide();

	var flag = true;
	var flag_spatialf = true;
	var flag_morphology = true;
	var flag_histogram = true;
	var flag_others = true;
	var flag_file_listing = true;
	var flag_noise = true;
	var flag_upload = true;


	$("button#cmd_toggle").click(function() {
		$("div.cmdoutput").slideToggle("slow");
		if(flag)
		{
			$("button#cmd_toggle").html("Show Terminal Output");
			flag = false;
		} else {
			$("button#cmd_toggle").html("Hide Terminal  Output");
			flag = true;
		}
	});
	
	$("h3#spatialf").click(function() {
		if(flag_spatialf)
		{
			$("h3#spatialf").html("Spatial Filtering << ");
			$("div#spatialf").slideDown("slow");
			flag_spatialf = false;
		} else {
			$("h3#spatialf").html("Spatial Filtering >> ");
			$("div#spatialf").slideUp("slow");
			flag_spatialf = true;
		}
	});

	$("h3#morphology").click(function() {
		if(flag_morphology)
		{
			$("h3#morphology").html("Morphological Filter Class << ");
			$("div#morphology").slideDown("slow");
			flag_morphology = false;
		} else {
			$("h3#morphology").html("Morphological Filter Class >> ");
			$("div#morphology").slideUp("slow");
			flag_morphology = true;
		}
	});

	$("h3#histogram").click(function() {
		if(flag_histogram)
		{
			$("h3#histogram").html("Histogram Based Processing << ");
			$("div#histogram").slideDown("slow");
			flag_histogram = false;	
		} else {
			$("h3#histogram").html("Histogram Based Processing >> ");
			$("div#histogram").slideUp("slow");
			flag_histogram = true;
		}	
	});

	$("h3#others").click(function() {
		if(flag_others)
		{
			$("h3#others").html("Others << ");
			$("div#others").slideDown("slow");
			flag_others = false;
		} else {
			$("h3#others").html("Others >> ");
			$("div#others").slideUp("slow");
			flag_others = true;
		}
	});

	$("h3#noise").click(function() {
		if(flag_noise)
		{
			$("h3#noise").html("Add Noise <<");
			$("div#noise").slideDown("slow");
			flag_noise = false;
		} else {
			$("h3#noise").html("Add Noise >>");
			$("div#noise").slideUp("slow");
			flag_noise = true;
		}
	});

	$("h3#file_listing").click(function() {
		if(flag_file_listing)
		{
			$("h3#file_listing").html("List/Change Image Files << ");
			$("div#file_listing").slideDown("slow");
			flag_file_listing = false;
		} else {
			$("h3#file_listing").html("List/Change Image Files >> ");
			$("div#file_listing").slideUp("slow");
			flag_file_listing = true;
		}
	});
			

	$("h3#upload").click(function() {
		if(flag_upload)
		{
			$("h3#upload").html("Upload PNG Image << ");
			$("div#upload").slideDown("slow");
			flag_upload = false;
		} else {
			$("h3#upload").html("Upload PNG Image >> ");
			$("div#upload").slideUp("slow");
			flag_upload = true;
		}
	});
});

