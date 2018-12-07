from django.db import models
from django.conf import settings


class Score(models.Model):
    score = models.IntegerField()
    user = models.OneToOneField(settings.AUTH_USER_MODEL, on_delete=models.CASCADE)

    def __str__(self):
        return '<%s:%d' % (self.user, self.score)


class Session(models.Model):
    user = models.ForeignKey(settings.AUTH_USER_MODEL, on_delete=models.CASCADE, null=True)
    code = models.CharField(blank=False, max_length=4)

class UserPress(models.Model):
    user = models.ForeignKey(settings.AUTH_USER_MODEL, on_delete=models.CASCADE, null=True)
    code = models.CharField(blank=False, max_length=4)
