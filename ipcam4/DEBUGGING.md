* apt list --installed "libqt5*" | cut -d/ -f1
* dpkg-query --showformat='${Package}\n' --show "linux-image*"
* dpkg-query --list "linux-image-*" | grep "ii"
* apt list --installed "linux-image-*" 2>/dev/null |awk -F'/' 'NR>1{print $1}'
* apt list --installed "linux-image-*" 2>/dev/null |awk -F'/' 'NR>1 && $0~/4.1/{print $1}'
* dpkg -l | grep ^ii | awk '{print $2}'