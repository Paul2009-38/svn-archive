from django.conf.urls.defaults import *
from tah.user.views import *

urlpatterns = patterns('tah.requests',
    (r'^$', index),
    (r'^show/$', show_user),
    (r'^show/(.+)/$', show_single_user),
    #(r'^login/$', login),
)


