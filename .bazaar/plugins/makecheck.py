from bzrlib import branch
 
def check_tests(local_branch, master_branch, old_revision_number, old_revision_id, future_revision_number, future_revision_id, tree_delta, future_tree):
    import os,subprocess
    from bzrlib import errors
 
    #Run the pre-commit script
    tests_result = subprocess.call("make check", shell=True)
 
    #If our script returns something other than 0, tests have failed
    if tests_result != 0:
        raise errors.BzrError("Tests failed, fix them before commit!")
 
#Install the hook with Bazaar
branch.Branch.hooks.install_named_hook('pre_commit', check_tests, 'Run make check pre-commit hook')
