BEGIN { fake=ORS; ORS="," }
$2 ~ /CPU/ {
print $3
}
END { ORS=fake; print ";\n" }
