#
## Makefile for MultiPath TCP support code.
#
#

obj-$(CONFIG_MPTCP) += mptcp.o

mptcp-y := hw_mptcp_ext.o mptcp_ctrl.o mptcp_ipv4.o mptcp_pm.o \
	   mptcp_output.o mptcp_input.o mptcp_sched.o

obj-$(CONFIG_TCP_CONG_LIA) += mptcp_coupled.o
obj-$(CONFIG_TCP_CONG_OLIA) += mptcp_olia.o
obj-$(CONFIG_TCP_CONG_WVEGAS) += mptcp_wvegas.o
obj-$(CONFIG_TCP_CONG_BALIA) += mptcp_balia.o
obj-$(CONFIG_MPTCP_FULLMESH) += mptcp_fullmesh.o
obj-$(CONFIG_MPTCP_NDIFFPORTS) += mptcp_ndiffports.o
obj-$(CONFIG_MPTCP_BINDER) += mptcp_binder.o
obj-$(CONFIG_MPTCP_ROUNDROBIN) += mptcp_rr.o
obj-$(CONFIG_MPTCP_REDUNDANT) += mptcp_redundant.o
obj-$(CONFIG_MPTCP_HEARTBEAT) += hw_mptcp_heartbeat.o
obj-$(CONFIG_MPTCP_EPC) += hw_mptcp_epc.o
mptcp-$(subst m,y,$(CONFIG_IPV6)) += mptcp_ipv6.o

