BEGIN { ORS="," }
$2 ~ /CPU/ {
print $3
}
END { print ";\n" }
