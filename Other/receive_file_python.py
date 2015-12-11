def commit_dynamic_picture(self, request):

        t_id = request.GET.get("t_id", "")
        rdm_id = request.GET.get("rdm_id", "")
        job_id = request.GET.get("job_id", "")
        product_id = request.GET.get("product_id", "")

        PATH_DIRECTORY = "D:\\Wor_Support_Directory\\Work_Support_Modify\\branches\\workbench_2.0\\apps\\build\\rdm_source\\templates"

        file_list = []
        for root, dirs, files in os.walk(PATH_DIRECTORY):
            for file in files:
                file_list.append(join(root, file))

        import re
        file_list_iter = file_list[:]
        for index, one_file in enumerate(file_list_iter):
            match_result = re.search("job_dynamic_picture", one_file)
            if match_result is None:
                file_list.remove(one_file)

        return render(request, "commit_dynamic_picture.html", locals())

    def rename_file(self ,request):

        selected_file_path = request.GET.get("selected_file_path", "")
        selected_file_path = selected_file_path.strip()
        modify_file_path = request.GET.get("modify_file_path", "")
        modify_file_path = modify_file_path.strip()

        #print "selected_file_path = ", selected_file_path, len(selected_file_path)
        #print "modify_file_path = ", modify_file_path, len(modify_file_path)

        import os
        if selected_file_path == "" or modify_file_path == "":
            return self.commit_dynamic_picture(request)
        else:
            selected_file_dir = os.path.dirname(selected_file_path)
            modify_file_dir = os.path.dirname(modify_file_path)
            #print "selected_file_dir = ", selected_file_dir
            #print "modify_file_dir = ", modify_file_dir

        if not os.path.exists(selected_file_path):
            pass
        elif selected_file_path == modify_file_path:
            #print "two file is equal"
            pass
        elif  selected_file_dir !=  modify_file_dir:
            #print "two files's directory is not equal"
            pass
        else:
            os.rename(selected_file_path, modify_file_path)

        url_access = self.redirect_to_method(request, "Involve", "commit_dynamic_picture")
        return HttpResponseRedirect(url_access)
        
        
def build_upload_file(request):

    import os

    PATH_DIRECTORY = "D:\\......."
    file_name = request.GET.get("fileName", "")

    file_path = os.path.join(PATH_DIRECTORY, file_name)

    #print "file_name = ", file_name
    #print request.POST
    #print len(request.FILES)
    data = request.read()
    #print "date's len", len(data)
    #print request.FILES["file"]

    f = open(file_path, "w")
    f.write(data)

    return HttpResponse("upload success")
