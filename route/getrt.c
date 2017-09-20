/* include getrt1 */
#include	"unproute.h"

#define	BUFLEN	(sizeof(struct rt_msghdr) + 512)
/* sizeof(struct sockaddr_in6) * 8 = 192 */
#define	SEQ		9999

// 路由套接字获取路由表信息 ???
int
main(int argc, char **argv)
{
	int					sockfd;
	char				*buf;
	pid_t				pid;
	ssize_t				n;
	struct rt_msghdr	*rtm;
	struct sockaddr		*sa, *rti_info[RTAX_MAX];
	struct sockaddr_in	*sin;

	if (argc != 2)
		err_quit("usage: getrt <IPaddress>");

	sockfd = Socket(AF_ROUTE, SOCK_RAW, 0);	/* need superuser privileges */

	buf = Calloc(1, BUFLEN);	/* and initialized to 0 */

	// construct req: rt_msghdr + sockaddr_in
	// req part1: rt_msghdr
	rtm = (struct rt_msghdr *) buf;
	rtm->rtm_msglen = sizeof(struct rt_msghdr) + sizeof(struct sockaddr_in);
	rtm->rtm_version = RTM_VERSION;
	rtm->rtm_type = RTM_GET;
	rtm->rtm_addrs = RTA_DST;
	rtm->rtm_pid = pid = getpid();
	rtm->rtm_seq = SEQ;

	// req part2: SA
	sin = (struct sockaddr_in *) (rtm + 1);
	sin->sin_len = sizeof(struct sockaddr_in);
	sin->sin_family = AF_INET;
	Inet_pton(AF_INET, argv[1], &sin->sin_addr);

	Write(sockfd, rtm, rtm->rtm_msglen); // request, 内核在rtm指针处填写返回信息

	// return res: rt_msghdr + sockaddr_in + sockaddr_in + ...
	do {
		n = Read(sockfd, rtm, BUFLEN); // read
	} while (rtm->rtm_type != RTM_GET || rtm->rtm_seq != SEQ ||
					 rtm->rtm_pid != pid);
/* end getrt1 */

/* include getrt2 */
	rtm = (struct rt_msghdr *) buf; // 返回信息既是在buf处
	sa = (struct sockaddr *) (rtm + 1);
	get_rtaddrs(rtm->rtm_addrs, sa, rti_info); // 根据掩码来拉多个地址sa放过到rti_info中
	if ( (sa = rti_info[RTAX_DST]) != NULL)
		printf("dest: %s\n", Sock_ntop_host(sa, sa->sa_len));

	if ( (sa = rti_info[RTAX_GATEWAY]) != NULL)
		printf("gateway: %s\n", Sock_ntop_host(sa, sa->sa_len));

	if ( (sa = rti_info[RTAX_NETMASK]) != NULL)
		printf("netmask: %s\n", Sock_masktop(sa, sa->sa_len));

	if ( (sa = rti_info[RTAX_GENMASK]) != NULL)
		printf("genmask: %s\n", Sock_masktop(sa, sa->sa_len));

	exit(0);
}
/* end getrt2 */
