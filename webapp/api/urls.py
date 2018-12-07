from django.urls import path
from .views import session, session_with_urlkwarg, score, user, userpress

urlpatterns = [
    path('session/', session, name='session'),
    path('session/<code>/', session_with_urlkwarg, name='session_with_urlkwarg'),
    path('score/<username>/', score, name='score'),
    path('user/<username>/userpress/', userpress, name='userpress'),
    path('user/<code>/', user, name='user'),
]
