#!/usr/bin/python
# Copyright 2016 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Author: Timo Jyrinki <timo.jyrinki@canonical.com>

from bzrlib import branch
from bzrlib import errors
import os
import subprocess


def pre_commit_hook(local, master, old_revno, old_revid, future_revno,
                    future_revid, tree_delta, future_tree):
    """Ensure packaging has gone through wrap-and-sort command"""

    if (master.get_parent() is None):
        return
    if (master.get_parent().find("ubuntu-ui-toolkit") == -1):
        return

    if not os.path.exists("/usr/bin/wrap-and-sort"):
        raise errors.BzrError("Please install 'devscripts' package.")
        return

    subprocess.call(["cp",
                     "-a",
                     "debian",
                     "debian-packaging-wraptest-temporary"])

    subprocess.call(["wrap-and-sort", "-a", "-t"])

    returncode = subprocess.call(["diff",
                                  "-urN",
                                  "debian-packaging-wraptest-temporary",
                                  "debian"])
    if returncode == 1:
        subprocess.call(["rm", "-rf", "debian-packaging-wraptest-temporary"])
        raise errors.BzrError("Please run wrap-and-sort\
                              -a -t to clean up packaging.")

    subprocess.call(["rm", "-rf", "debian-packaging-wraptest-temporary"])

branch.Branch.hooks.install_named_hook("pre_commit",
                                       pre_commit_hook,
                                       "Check packaging sorting")
