from django.contrib.auth.mixins import LoginRequiredMixin
from django.contrib.auth.forms import UserCreationForm
from django.views.generic.list import ListView
from django.views.generic.edit import CreateView
from django.views.generic import TemplateView
from django.contrib.auth import login, logout, authenticate
from django.shortcuts import render, redirect
from django.urls import reverse, reverse_lazy
from scores.models import Score, Session, UserPress

def logout_view(request):
    logout(request)
    return redirect('user-home')

def signup(request):
    if request.method == 'POST':
        print('form posted', request)
        form = UserCreationForm(request.POST)
        if form.is_valid():
            print('form is valid')
            form.save()
            username = form.cleaned_data.get('username')
            raw_password = form.cleaned_data.get('password1')
            user = authenticate(username=username, password=raw_password)
            login(request, user)
            return redirect('user-home')
    else:
        form = UserCreationForm()
    return render(request, 'index/signup.html', {'form': form})

class UserHome(TemplateView):
    template_name = 'index/user_home.html'

    def get_context_data(self, *args, **kwargs):
        context = super().get_context_data(*args, **kwargs)
        context['user'] = self.request.user
        if self.request.user.is_authenticated:
            context['start_game_url'] = reverse('button-input')
            try:
                context['score'] = Score.objects.get(user=self.request.user)
            except Score.DoesNotExist:
                context['score'] = Score.objects.create(user=self.request.user, score=0)

        return context

class ButtonInputView(LoginRequiredMixin, CreateView, TemplateView):
    template_name = 'index/button_input.html'
    model = Session
    fields = ['user']
    object = None

    def post(self, *args, **kwargs):
        code = self.request.POST['hidden']
        try:
            session = Session.objects.get(code=code)
            if session.user:
                return redirect('session-in-progress')

            session.user = self.request.user
            session.save()
            return redirect('code-valid')
        except Session.DoesNotExist:
            """Show screen that session does not exist, redirect to button-input"""
            print('Session does not exist, redirect')
            return redirect('code-invalid')
        return super().post(*args, **kwargs)

    def get_context_data(self, *args, **kwargs):
        context = super().get_context_data(*args, **kwargs)
        context['user'] = self.request.user
        return context

class CustomRedirectView(LoginRequiredMixin, TemplateView):
    def get_context_data(self, *args, **kwargs):
        context = super().get_context_data(*args, **kwargs)
        context['redirect_url'] = self.redirect_url
        return context

class CodeInvalidView(CustomRedirectView):
    template_name = 'index/code_invalid.html'
    redirect_url = reverse_lazy('button-input')

class CodeValidView(CustomRedirectView):
    template_name = 'index/code_valid.html'
    redirect_url = reverse_lazy('user-home')

class SessionInProgress(CustomRedirectView):
    template_name = 'index/session_in_progress.html'
    redirect_url = reverse_lazy('button-input')

class ScoreListView(ListView):
    template_name = 'index/score_list.html'
    model = Score
    paginate_by = 10
    queryset = Score.objects.all().order_by('-score')

class GamePlayView(LoginRequiredMixin, CreateView, TemplateView):
    template_name = 'index/gameplay.html'
    model = Session
    fields = ['user']
    object = None

    def post(self, *args, **kwargs):
        code = self.request.POST['hidden']
        press, _ = UserPress.objects.get_or_create(user=self.request.user)
        press.code = code
        press.save()
        return super().post(*args, **kwargs)

    def get_context_data(self, *args, **kwargs):
        context = super().get_context_data(*args, **kwargs)
        context['user'] = self.request.user
        return context
