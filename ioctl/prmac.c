#include	"unpifi.h"
#include	<net/if_arp.h>

// 获取arp interface信息
int
main(int argc, char **argv)
{
	int					sockfd;
	struct ifi_info			*ifi;
	unsigned char		*ptr;
	struct arpreq		arpreq;
	struct sockaddr_in	*sin;

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
	for (ifi = get_ifi_info(AF_INET, 0); ifi != NULL; ifi = ifi->ifi_next) {
		printf("%s: ", Sock_ntop(ifi->ifi_addr, sizeof(struct sockaddr_in)));

		// 对每一个interface，用其ifi_info.ifi_addr放到arpreq中去获得该地址的arp信息
		sin = (struct sockaddr_in *) &arpreq.arp_pa;
		memcpy(sin, ifi->ifi_addr, sizeof(struct sockaddr_in));

		if (ioctl(sockfd, SIOCGARP, &arpreq) < 0) {
			err_ret("ioctl SIOCGARP");
			continue;
		}

		ptr = &arpreq.arp_ha.sa_data[0];
		printf("%x:%x:%x:%x:%x:%x\n", *ptr, *(ptr+1),
			   *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5));
	}
	exit(0);
}
