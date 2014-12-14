/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fb.h>
#include <linux/notifier.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/debugfs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/iopoll.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>

#include "mdss_fb.h"
#include "mdss_dsi.h"
#include "mdss_panel.h"
#include "mdss_mdp.h"
#include "mdp3_ctrl.h"

<<<<<<< HEAD
#define STATUS_CHECK_INTERVAL 8000
#define STATUS_CHECK_INTERVAL_MIN 200

struct dsi_status_data {
	struct notifier_block fb_notifier;
	struct delayed_work check_status;
	struct msm_fb_data_type *mfd;
};
=======
#define STATUS_CHECK_INTERVAL_MS 5000
#define STATUS_CHECK_INTERVAL_MIN_MS 200
#define DSI_STATUS_CHECK_DISABLE 0

static uint32_t interval = STATUS_CHECK_INTERVAL_MS;
static uint32_t dsi_status_disable = DSI_STATUS_CHECK_DISABLE;
>>>>>>> caf/LA.BF.1.1_rb1.9
struct dsi_status_data *pstatus_data;

/*
 * check_dsi_ctrl_status() - Reads MFD structure and
 * calls platform specific DSI ctrl Status function.
 * @work  : dsi controller status data
 */
static void check_dsi_ctrl_status(struct work_struct *work)
{
	struct dsi_status_data *pdsi_status = NULL;
<<<<<<< HEAD
	struct mdss_panel_data *pdata = NULL;
	struct mdss_dsi_ctrl_pdata *ctrl_pdata = NULL;
#ifdef CONFIG_FB_MSM_MDSS_MDP3
	struct mdp3_session_data *mdp3_session = NULL;
#else
	struct mdss_overlay_private *mdp5_data = NULL;
	struct mdss_mdp_ctl *ctl = NULL;
#endif
	int ret = 0;
=======
>>>>>>> caf/LA.BF.1.1_rb1.9

	pdsi_status = container_of(to_delayed_work(work),
		struct dsi_status_data, check_status);

	if (!pdsi_status) {
		pr_err("%s: DSI status data not available\n", __func__);
		return;
	}

	if (!pdsi_status->mfd) {
		pr_err("%s: FB data not available\n", __func__);
		return;
	}

<<<<<<< HEAD
	ctrl_pdata = container_of(pdata, struct mdss_dsi_ctrl_pdata,
							panel_data);
	if (!ctrl_pdata || !ctrl_pdata->check_status) {
		pr_err("%s: DSI ctrl or status_check callback not available\n",
								__func__);
		return;
	}

#ifdef CONFIG_FB_MSM_MDSS_MDP3
	mdp3_session = pdsi_status->mfd->mdp.private1;
	mutex_lock(&mdp3_session->offlock);
	ret = ctrl_pdata->check_status(ctrl_pdata);
	mutex_unlock(&mdp3_session->offlock);
#else

	if (!pdata->panel_info.cont_splash_esd_rdy) {
		pr_warn("%s: Splash not complete, reschedule check status\n",
			__func__);
		schedule_delayed_work(&pdsi_status->check_status,
				msecs_to_jiffies(interval));
		return;
	}

	mdp5_data = mfd_to_mdp5_data(pdsi_status->mfd);
	ctl = mfd_to_ctl(pdsi_status->mfd);
	if (!ctl) {
		pr_warn("%s: mdss_mdp_ctl data not available\n", __func__);
		return;
	}

	mutex_lock(&ctl->offlock);
	if (ctl->shared_lock)
		mutex_lock(ctl->shared_lock);

	mdss_mdp_clk_ctrl(MDP_BLOCK_POWER_ON, false);
	ret = ctrl_pdata->check_status(ctrl_pdata);
	mdss_mdp_clk_ctrl(MDP_BLOCK_POWER_OFF, false);

	if (ctl->shared_lock)
		mutex_unlock(ctl->shared_lock);
	mutex_unlock(&ctl->offlock);
#endif

	if ((pdsi_status->mfd->panel_power_on)) {
		if (ret > 0) {
			schedule_delayed_work(&pdsi_status->check_status,
				msecs_to_jiffies(interval));
		} else {
			char *envp[2] = {"PANEL_ALIVE=0", NULL};
			pdata->panel_info.panel_dead = true;
			ret = kobject_uevent_env(
				&pdsi_status->mfd->fbi->dev->kobj,
							KOBJ_CHANGE, envp);
			pr_err("%s: Panel has gone bad, sending uevent - %s\n",
							__func__, envp[0]);
		}
	}
=======
	pdsi_status->mfd->mdp.check_dsi_status(work, interval);
>>>>>>> caf/LA.BF.1.1_rb1.9
}

/*
 * fb_event_callback() - Call back function for the fb_register_client()
 *			 notifying events
 * @self  : notifier block
 * @event : The event that was triggered
 * @data  : Of type struct fb_event
 *
 * This function listens for FB_BLANK_UNBLANK and FB_BLANK_POWERDOWN events
 * from frame buffer. DSI status check work is either scheduled again after
 * PANEL_STATUS_CHECK_INTERVAL or cancelled based on the event.
 */
static int fb_event_callback(struct notifier_block *self,
				unsigned long event, void *data)
{
	struct fb_event *evdata = data;
	struct dsi_status_data *pdata = container_of(self,
				struct dsi_status_data, fb_notifier);
	struct mdss_dsi_ctrl_pdata *ctrl_pdata = NULL;
<<<<<<< HEAD
=======
	struct mdss_panel_info *pinfo;
>>>>>>> caf/LA.BF.1.1_rb1.9

	pdata->mfd = evdata->info->par;
	ctrl_pdata = container_of(dev_get_platdata(&pdata->mfd->pdev->dev),
				struct mdss_dsi_ctrl_pdata, panel_data);
	if (!ctrl_pdata) {
		pr_err("%s: DSI ctrl not available\n", __func__);
		return NOTIFY_BAD;
	}
<<<<<<< HEAD
	if (ctrl_pdata->check_status_disabled) {
		pr_debug("%s: status_check disabled\n", __func__);
=======

	pinfo = &ctrl_pdata->panel_data.panel_info;

	if (!(pinfo->esd_check_enabled)) {
		pr_debug("ESD check is not enaled in panel dtsi\n");
		return NOTIFY_DONE;
	}

	if (dsi_status_disable) {
		pr_debug("%s: DSI status disabled\n", __func__);
>>>>>>> caf/LA.BF.1.1_rb1.9
		return NOTIFY_DONE;
	}

	if (event == FB_EVENT_BLANK && evdata) {
		int *blank = evdata->data;
		struct dsi_status_data *pdata = container_of(self,
				struct dsi_status_data, fb_notifier);
		pdata->mfd = evdata->info->par;

		switch (*blank) {
		case FB_BLANK_UNBLANK:
			schedule_delayed_work(&pdata->check_status,
				msecs_to_jiffies(interval));
			break;
		case FB_BLANK_POWERDOWN:
		case FB_BLANK_HSYNC_SUSPEND:
		case FB_BLANK_VSYNC_SUSPEND:
		case FB_BLANK_NORMAL:
			cancel_delayed_work(&pdata->check_status);
			break;
		default:
			pr_err("Unknown case in FB_EVENT_BLANK event\n");
			break;
		}
	}
	return 0;
}

<<<<<<< HEAD
=======
static int param_dsi_status_disable(const char *val, struct kernel_param *kp)
{
	int ret = 0;
	int int_val;

	ret = kstrtos32(val, 0, &int_val);
	if (ret)
		return ret;

	pr_info("%s: Set DSI status disable to %d\n",
			__func__, int_val);
	*((int *)kp->arg) = int_val;
	return ret;
}

>>>>>>> caf/LA.BF.1.1_rb1.9
static int param_set_interval(const char *val, struct kernel_param *kp)
{
	int ret = 0;
	int int_val;

	ret = kstrtos32(val, 0, &int_val);
	if (ret)
		return ret;
<<<<<<< HEAD
	if (int_val < STATUS_CHECK_INTERVAL_MIN) {
=======
	if (int_val < STATUS_CHECK_INTERVAL_MIN_MS) {
>>>>>>> caf/LA.BF.1.1_rb1.9
		pr_err("%s: Invalid value %d used, ignoring\n",
						__func__, int_val);
		ret = -EINVAL;
	} else {
		pr_info("%s: Set check interval to %d msecs\n",
						__func__, int_val);
		*((int *)kp->arg) = int_val;
	}
	return ret;
}

int __init mdss_dsi_status_init(void)
{
	int rc = 0;

	pstatus_data = kzalloc(sizeof(struct dsi_status_data), GFP_KERNEL);
	if (!pstatus_data) {
		pr_err("%s: can't allocate memory\n", __func__);
		return -ENOMEM;
	}

	pstatus_data->fb_notifier.notifier_call = fb_event_callback;

	rc = fb_register_client(&pstatus_data->fb_notifier);
	if (rc < 0) {
		pr_err("%s: fb_register_client failed, returned with rc=%d\n",
								__func__, rc);
		kfree(pstatus_data);
		return -EPERM;
	}

	pr_info("%s: DSI status check interval:%d\n", __func__,	interval);

	INIT_DELAYED_WORK(&pstatus_data->check_status, check_dsi_ctrl_status);

	pr_debug("%s: DSI ctrl status work queue initialized\n", __func__);

	return rc;
}

void __exit mdss_dsi_status_exit(void)
{
	fb_unregister_client(&pstatus_data->fb_notifier);
	cancel_delayed_work_sync(&pstatus_data->check_status);
	kfree(pstatus_data);
	pr_debug("%s: DSI ctrl status work queue removed\n", __func__);
}

module_param_call(interval, param_set_interval, param_get_uint,
<<<<<<< HEAD
							&interval, 0644);
=======
						&interval, 0644);
>>>>>>> caf/LA.BF.1.1_rb1.9
MODULE_PARM_DESC(interval,
		"Duration in milliseconds to send BTA command for checking"
		"DSI status periodically");

module_param_call(dsi_status_disable, param_dsi_status_disable, param_get_uint,
						&dsi_status_disable, 0644);
MODULE_PARM_DESC(dsi_status_disable,
		"Disable DSI status check");

module_init(mdss_dsi_status_init);
module_exit(mdss_dsi_status_exit);

MODULE_LICENSE("GPL v2");
