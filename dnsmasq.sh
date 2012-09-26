#!/bin/bash
# run dnsmasq and redirect name address to local ip address

is_ip_exist=""

while [ "$is_ip_exist" == "" ]
do
    is_ip_exist=`LC_ALL=C ifconfig  | grep 'inet addr:'| grep -v '127.0.0.1'`
    if [ "$is_ip_exist" == "" ]; then
        echo "Waiting network startup"
        sleep 2
    fi
done

ip=`echo $is_ip_exist | cut -d: -f2 | awk '{ print $1}'`
echo "find local ip:$ip"

# kill dnsmasq
pid=`pidof dnsmasq`
if [ "$pid" != "" ]; then
    sudo kill -9 $pid
fi

add_redirect=""
address="www.baidu.com www.google.com.hk"
for add in $address
do
    add_redirect+="--address=/$add/$ip "
done
echo $add_redirect

# dnsmasq options: no-resolv no-poll server=8.8.8.8 no-host
# --no-resolv --no-poll --no-host --server=8.8.8.8 --address=/www.baidu.com/127.0.0.1 --log-queries
# --log-facility=/var/log/dnsmasq.log

sudo dnsmasq --no-resolv --no-poll --no-host --log-queries --log-facility=/var/log/dnsmasq.log --server=202.101.172.35 $add_redirect
