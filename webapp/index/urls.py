from django.urls import path
from .views import signup, UserHome, ButtonInputView, CodeInvalidView, CodeValidView, SessionInProgress, logout_view, ScoreListView, GamePlayView

urlpatterns = [
    path('', UserHome.as_view(), name='root'),
    path('signup', signup, name='signup'),
    path('logout', logout_view, name='logout'),
    path('home', UserHome.as_view(), name='user-home'),
    path('button-input', ButtonInputView.as_view(), name='button-input'),
    path('code-invalid', CodeInvalidView.as_view(), name='code-invalid'),
    path('code-valid', CodeValidView.as_view(), name='code-valid'),
    path('session-in-progress', SessionInProgress.as_view(), name='session-in-progress'),
    path('score-list', ScoreListView.as_view(), name='score-list'),
    path('gameplay', GamePlayView.as_view(), name='gameplay'),
]
