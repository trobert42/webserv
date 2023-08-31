<?php

$entityBody = file_get_contents('php://input');
parse_str($entityBody, $BODY);
print_r($BODY);

?>
