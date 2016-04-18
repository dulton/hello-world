<html>
<head><title>Personality</title></head>
<body>
  
<?php
 // fetch form values, if any
 $attrs = $_GET['attributes'];
 if (! is_array($attrs)) { $attrs = array( ); }
  
 // create HTML for identically-named checkboxes
  
 function make_checkboxes ($name, $query, $options) {
   foreach ($options as $value => $label) {
     printf('%s <input type="checkbox" name="%s[]" value="%s" ',
             $label, $name, $value);
     if (in_array($value, $query)) { echo "checked "; }
     echo "/><br />\n";
   }
 }
  
 // the list of values and labels for the checkboxes
 $personality_attributes = array(
   'perky'    => 'Perky',
   'morose'   => 'Morose',
   'thinking' => 'Thinking',
   'feeling'  => 'Feeling',
   'thrifty'  => 'Spend-thrift',
   'prodigal' => 'Shopper'
 );
?>
  
<form action="<?php $_SERVER['PHP_SELF'] ?>" method="GET">
Select your personality attributes:<br />
<?php make_checkboxes('attributes', $attrs, $personality_attributes); ?>
<br />
<input type="submit" name="s" value="Record my personality!" />
</form>

<?php
 if (array_key_exists('s', $_GET)) {
   $description = join (" ", $_GET['attributes']);
   echo "You have a $description personality.";
 }
?>
  
</body>
</html>
